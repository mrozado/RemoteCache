#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <mcheck.h>
#include <memcached/engine.h>
#include <memcached/util.h>
#include <memcached/visibility.h>
#include <memcached/config_parser.h>
#include "commons/config.h"
#include <semaphore.h>
#include "commons/bitarray.h"
#include <sys/mman.h>

#include "./DataStructures.h"
#include "./MemcachedInterface.h"
#include "./ParticionesDinamicas.h"
#include "./AllocationAlgorithms.h"
#include "./ReplacementAlgorithms.h"
#include "./helpers.h"

#define CONFIG_PATH "Config.txt"

MemoryStructures Cache;
t_config *config;

sem_t *readSem;
sem_t *writeSem;
sem_t *readSemBitArray;
sem_t *writeSemBitArray;


MEMCACHED_PUBLIC_API ENGINE_ERROR_CODE create_instance(uint64_t interface, GET_SERVER_API get_server_api, ENGINE_HANDLE **handle)
{
	if (interface == 0)
		return ENGINE_ENOTSUP;

	EngineHandle *engine = calloc(1, sizeof(EngineHandle));
	if (engine == NULL)
		return ENGINE_ENOMEM;

	engine->engine.interface.interface = 1;

	engine->engine.initialize = CacheInitialize;
	engine->engine.destroy = CacheDestroy;
	engine->engine.get_info = CacheGetInfo;
	engine->engine.allocate = CacheAllocate;
	engine->engine.remove = CacheItemDelete;
	engine->engine.release = CacheItemRelease;
	engine->engine.get = CacheGet;
	engine->engine.get_stats = CacheGetStats;
	engine->engine.reset_stats = CacheResetStats;
	engine->engine.store = CacheStore;
	engine->engine.flush = CacheFlush;
	engine->engine.unknown_command = CacheUnknownCommand;
	engine->engine.item_set_cas = CacheItemSetCas;
	engine->engine.get_item_info = CacheGetItemInfo;

	//control


	engine->get_server_api = get_server_api;

	*handle = (ENGINE_HANDLE*) engine;

	return ENGINE_SUCCESS;
}

static void CacheDummp(int isignal)
{
	signal(SIGUSR1, CacheDummp);
	FILE* fileDecriptor = fopen("MemoryDump.txt", "w");

		//DumpBitArray(fileDecriptor);
		DumpMemoryTable(fileDecriptor);
		DumpFreeBlocks(fileDecriptor);

	fclose(fileDecriptor);



}
static void CacheDestroy(ENGINE_HANDLE* handle, const bool force)
{
	sem_wait(readSem);
	sem_wait(writeSem);
	ReleaseBitArray();
	ReleaseMemoryTable();
	ReleaseDataRegion();
	free(handle);
	munlockall();
	muntrace();
	free(handle);
	sem_post(readSem);
	sem_post(writeSem);
}
static void CacheItemRelease(ENGINE_HANDLE *handler, const void *cookie, item* item)
{
//	DataDecriptor* castedItem = (DataDecriptor*) item;
//	if (!castedItem->IsStored)
//	{
//		sem_wait(readSem);
//		sem_wait(writeSem);
//
//		castedItem->Data = NULL;
//		castedItem->DataSize = 0;
//		castedItem->DataFSSize = 0;
//		castedItem->HashKey = 0;
//		castedItem->IsStored = false;
//		free(castedItem);
//		sem_post(readSem);
//		sem_post(writeSem);
//	}
}
static bool CacheGetItemInfo(ENGINE_HANDLE *handler, const void *cookie, const item* item, item_info* returnItem)
{
	// casteamos de item*, el cual es la forma generica en la cual memcached trata a nuestro tipo de item, al tipo
	// correspondiente que nosotros utilizamos
	DataDecriptor* castedItem = (DataDecriptor*) item;

	if (returnItem->nvalue < 1)
		return false;

	returnItem->cas = 0; /* Not supported */
	returnItem->clsid = 0; /* Not supported */
	returnItem->exptime = 0;
	returnItem->flags = 0;
	returnItem->key = &castedItem->RealKey;

	returnItem->nkey = sizeof(castedItem->RealKey);
	returnItem->nbytes = castedItem->DataSize; /* Total length of the items data */
	returnItem->nvalue = 1; /* Number of fragments used ( Default ) */
	returnItem->value[0].iov_base = castedItem->Data; /* Hacemos apuntar item_info al comienzo de la info */
	returnItem->value[0].iov_len = castedItem->DataSize; /* Le seteamos al item_info el tamaño de la información */

	return true;
}

static const engine_info* CacheGetInfo(ENGINE_HANDLE* handle)
{
	static engine_info info =
	{ .description = "C lo que no hicimos el fuselage pasado - CACHE", .num_features = 0, .features =
	{ [0].feature = ENGINE_FEATURE_LRU, [0].description =
			"Fiesta" } };

	return &info;
}

static ENGINE_ERROR_CODE CacheInitialize(ENGINE_HANDLE* handle, const char* config_str)
{
	signal(SIGUSR1, CacheDummp);
	EngineHandle* engine = (EngineHandle*) handle;
	mtrace();
	/*
	 * En la variable config_str nos llega la configuración en el formato:
	 *
	 * 	cache_size=1024;chunk_size=48; etc ...
	 *
	 * La función parse_config recibe este string y una estructura que contiene
	 * las keys que debe saber interpretar, los tipos y los punteros de las variables donde tiene
	 * que almacenar la información.
	 *
	 */
	ConfigureCache(config_str, engine);
	readSem = malloc(sizeof(sem_t));
	writeSem = malloc(sizeof(sem_t));
	sem_init(readSem,0,1);
	sem_init(writeSem,0,1);
	readSemBitArray = malloc(sizeof(sem_t));
	writeSemBitArray = malloc(sizeof(sem_t));
	sem_init(readSemBitArray,0,1);
	sem_init(writeSemBitArray,0,1);
	InitializeMemorySpace(engine);

	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheAllocate(ENGINE_HANDLE *handler, const void* cookie, item** returnItem, const void* key, const size_t keySize, const size_t dataSize, const int flags, const rel_time_t expirationTime)
{
	// Allocate reserva (u obtiene) un espacio de memoria en el cual se va a almacenar transitoriamente
	// la informacion del item que se quiere guardar u obtener
	double HashKey = ToHashKey(key, keySize);
	char *prueba = (char*)key;
	DataDecriptor* existentItem = DeleteGet(HashKey);
	if (existentItem != NULL){

		Delete(existentItem);

	}

	DataDecriptor* item = Allocate(dataSize);
	if (item == NULL)
		return ENGINE_ENOMEM;

	item->IsStored = false;
	item->HashKey = HashKey;
	item->Useage = 1;
	//item->Key = (char*)key;

	//strcpy(item->RealKey,(char*)key);
	memcpy(item->RealKey, key, (keySize < KEY_SIZE) ? keySize : KEY_SIZE);
//	if(keySize<KEY_SIZE)
//		strcpy(item->RealKey[keySize],"\0");
	*returnItem = item;
	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheGet(ENGINE_HANDLE *handle, const void* cookie, item** itemToRetrive, const void* key, const int keySize, uint16_t vbucket)
{
	char *prueba = (char*)key;
	double HashKey = ToHashKey(key, keySize);
	DataDecriptor* item = Get(HashKey);
	if (item == NULL)
		return ENGINE_KEY_ENOENT;

	*itemToRetrive = item;
	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheStore(ENGINE_HANDLE *handle, const void *cookie, item* itemToStore, uint64_t *cas, ENGINE_STORE_OPERATION operation, uint16_t vbucket)
{
	//DataDecriptor *item = (DataDecriptor*) itemToStore;

	//item->IsStored = true;

	*cas = 0;
	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheFlush(ENGINE_HANDLE* handle, const void* cookie, time_t when)
{
	Flush();
	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheItemDelete(ENGINE_HANDLE* handle, const void* cookie, const void* key, const size_t nkey, uint64_t cas, uint16_t vbucket)
{
	double HashKey = ToHashKey(key, nkey);
	DataDecriptor* item = DeleteGet(HashKey);
	if (item == NULL)
		return ENGINE_KEY_ENOENT;
	Delete(item);
	return ENGINE_SUCCESS;
}

/*
 Dummy functions
 */
static ENGINE_ERROR_CODE CacheGetStats(ENGINE_HANDLE* handle,
		const void* cookie, const char* stat_key, int nkey, ADD_STAT add_stat)
{
	return ENGINE_SUCCESS;
}
static ENGINE_ERROR_CODE CacheUnknownCommand(ENGINE_HANDLE* handle,
		const void* cookie, protocol_binary_request_header *request,
		ADD_RESPONSE response)
{
	return ENGINE_ENOTSUP;
}
static void CacheResetStats(ENGINE_HANDLE* handle, const void *cookie)
{
}
static void CacheItemSetCas(ENGINE_HANDLE *handle, const void *cookie, item* item, uint64_t val)
{

}

/*
 Non-Memcached Functions
 */
static void ConfigureCache(const char* configurationString, EngineHandle* engine)
{

	if (configurationString != NULL)
	{
		struct config_item settings[] =
		{
			 {
				.key = "chunk_size",
				.datatype = DT_SIZE,
				.value.dt_size = &engine->config.chunk_size
			}, {
				.key = "item_size_max",
				.datatype = DT_SIZE,
				.value.dt_size = &engine->config.cache_max_size
			}, { .key = NULL }
		};

		parse_config(configurationString, settings, NULL);
	}

	/*
	 * Registro la SIGUSR1. El registro de signals debe ser realizado en la función initialize
	 */

}

static void InitializeMemorySpace(EngineHandle* engine)
{
	config = config_create(CONFIG_PATH);
	InitializeMemoryTable(engine);
	InitializeDataRegion(engine);
	InitializeBitArray(engine);
	strcpy(Cache.AlgorithmSelected ,config_get_string_value(config,"algorithm"));
	ConfigureAllocationAlgorithm(config_get_int_value(config,"allocation"));
	ConfigureReplacementAlgorithm(config_get_int_value(config,"replacement"));

	Flush();
}

static void InitializeMemoryTable(EngineHandle* engine)
{
	int TotalMemory = engine->config.cache_max_size;
	int MinPartitionSize = engine->config.chunk_size;

	int TotalPartitions = TotalMemory / MinPartitionSize;

	Cache.MemoryTable.Size = TotalPartitions;
	Cache.MemoryTable.ItemSize = sizeof(DataDecriptor);
	Cache.MemoryTable.Data = malloc(sizeof(DataDecriptor) * TotalPartitions);
	mlock(Cache.MemoryTable.Data,sizeof(DataDecriptor) * TotalPartitions);
}

static void ReleaseMemoryTable()
{
	free(Cache.MemoryTable.Data);
	Cache.MemoryTable.Size=0;
	Cache.MemoryTable.ItemSize=0;

}

static void InitializeDataRegion(EngineHandle* engine)
{
	int TotalMemory = engine->config.cache_max_size;

	Cache.DataRegion.Size = TotalMemory;
	Cache.DataRegion.ItemSize = 1;
	Cache.DataRegion.Data = malloc(Cache.DataRegion.Size);
	Cache.MinAllocationSize = engine->config.chunk_size;
	mlock(Cache.DataRegion.Data,Cache.DataRegion.Size);
}

static void ReleaseDataRegion(void)
{
	free(Cache.DataRegion.Data);
	Cache.DataRegion.Size=0;
	Cache.DataRegion.ItemSize=0;

}
static void InitializeBitArray(EngineHandle* engine)
{
	int TotalMemory = engine->config.cache_max_size;
	Cache.BitArray = *bitarray_create(malloc(TotalMemory / 8),TotalMemory / 8);
	mlock(Cache.BitArray.bitarray,Cache.BitArray.size);
}

void ReleaseBitArray(void)
{
	bitarray_destroy(&Cache.BitArray);
}

static void ConfigureAllocationAlgorithm(int algorithmNumber)
{

	//char *algoritmo = config_get_string_value(config,"algorithm") ;
	if(!strcmp(Cache.AlgorithmSelected,"pd")){
		switch (algorithmNumber) {
			case ALLOCATION_FIRSTFIT: Cache.AllocationAlgorithm = &FirstFit; break;
			case ALLOCATION_BESTFIT: Cache.AllocationAlgorithm = &BestFit; break;

		}
	}else if(!strcmp(Cache.AlgorithmSelected,"bs"))
	{
		switch (algorithmNumber) {
			case ALLOCATION_FIRSTFIT: Cache.AllocationAlgorithm = &FirstFitBS; break;
			case ALLOCATION_BESTFIT: Cache.AllocationAlgorithm = &BestFitBS; break;
		}
	}
}
static void ConfigureReplacementAlgorithm(int algorithmNumber)
{
	switch (algorithmNumber) {
		case REPLACEMENT_FIFO:
			Cache.ReplacementAlgorithm = &FIFO;
			break;
		case REPLACEMENT_LRU:
			Cache.ReplacementAlgorithm = &LRU;
			break;
	}

	free(config->properties);
	free(config->path);
	free(config);
}

//static double ToHashKey(const char* key,const size_t keySize)
//{
//	char fullKey[MAX_KEY_SIZE];
//	double hashedKey = 0;
//	int* byteGroups = NULL;
//	int byteGroupsSize = MAX_KEY_SIZE / sizeof(int);
//
//	memset(fullKey, '\0', MAX_KEY_SIZE);
//	memcpy(fullKey, key, keySize);
//	byteGroups = (int *)fullKey;
//
//	for(int i = 0; i < byteGroupsSize; i++)
//		hashedKey = hashedKey + byteGroups[i];
//
//	return hashedKey;
//}
