/*
 * bistTest.c
 *
 *  Created on: Jun 17, 2021
 *      Author: aL
 */
#include "bistTest.h"

const int numberOfPatternPasses = 50;
enum bistErrors {
	bistSuccess = 0,
	bistErrorId,
};


#define bistId 0x2c001fff
#define bistCacheAddress 0xbff0f000
#define bistMkioAddress 0xbff0f004
#define bistSwAddress 0xbff0f008
#define bistUartAddress 0xbff0f00c
#define bistSpiAddress 0xbff0f010
#define bistIdAddress 0xbff0f07c


typedef struct {
	unsigned int Bist_En 		:1;//1 - enable BIST
	unsigned int RunBist 		:1;
	unsigned int mode_test 		:1;//0 - stop on error, 1 - dont stop on error
	unsigned int Pattern 		:2;
	unsigned int Rd_Wr	 		:1;
	unsigned int Alg	 		:3;
	unsigned int Mask_Pbuf 		:1;
	unsigned int Mask_Dtag 		:1;
	unsigned int Mask_Itag 		:1;
	unsigned int Mask_Dcache	:1;
	unsigned int Mask_Icache	:1;
	unsigned int Nway	 		:1;
	unsigned int reserve 		:1;
	unsigned int Address 		:10;
	unsigned int Error	 		:5;
	unsigned int Done 		  	:1;
} __attribute__ ((__packed__)) BISTCACHE_UNIT;

typedef struct {
	unsigned int Bist_En 		:1;//1 - enable BIST
	unsigned int RunBist 		:1;
	unsigned int mode_test 		:1;//0 - stop on error, 1 - dont stop on error
	unsigned int Pattern 		:2;
	unsigned int Rd_Wr	 		:1;
	unsigned int Alg	 		:3;
	unsigned int reserve2 		:1;
	unsigned int Mask_mkio2 	:2;
	unsigned int Mask_mkio1		:2;
	unsigned int N_port			:1;
	unsigned int reserve 		:3;
	unsigned int Address 		:11;
	unsigned int Error	 		:2;
	unsigned int Done 		  	:1;
} __attribute__ ((__packed__)) BISTMKIO_UNIT;

typedef struct {
	unsigned int Bist_En 		:1;//1 - enable BIST
	unsigned int RunBist 		:1;
	unsigned int mode_test 		:1;//0 - stop on error, 1 - dont stop on error
	unsigned int Pattern 		:2;
	unsigned int Rd_Wr	 		:1;
	unsigned int Alg	 		:3;
	unsigned int reserve	 	:2;
	unsigned int Address		:4;
	unsigned int reserve2	 	:11;
	unsigned int Error	 		:4;
	unsigned int Done 		  	:1;
} __attribute__ ((__packed__)) BISTUART_UNIT;

typedef struct {
	unsigned int Bist_En 		:1;//1 - enable BIST
	unsigned int RunBist 		:1;
	unsigned int mode_test 		:1;//0 - stop on error, 1 - dont stop on error
	unsigned int Pattern 		:2;
	unsigned int Rd_Wr	 		:1;
	unsigned int Alg	 		:3;
	unsigned int rxtx	 		:1;
	unsigned int reserve	 	:2;
	unsigned int Address		:8;
	unsigned int Error	 		:11;
	unsigned int Done 		  	:1;
} __attribute__ ((__packed__)) BISTSW_UNIT;


int bistTestCache()
{
	int counter = 0;
	int errorCounter = 0;
	int passCounter = 0;
	int testCounter;
	tos("\n =========== Bist test Cache: ");

	BISTCACHE_UNIT * bistCache = (BISTCACHE_UNIT*) bistCacheAddress;

	//test MARCH/MATS++

	tos("\n Bist test March Test ");

	for(testCounter = 0; testCounter < 4;testCounter++){
		counter=0;
		*((unsigned int *)bistCache) = 0;
		bistCache->Bist_En = 1;
		bistCache->mode_test = 1;
		bistCache->Alg = 1<<testCounter;
		if(testCounter == 3)
			bistCache->Alg = 7;

		bistCache->RunBist = 1;

		while (bistCache->Done == 0){
			counter++;
		}
		if(bistCache->Error){
			*((unsigned int *)bistCache) = 0;//reset bist register
			tos("\n Bist test failure! ");
			return bistCache->Error;
		}
		tos("\n Pattern: ");
		ton(testCounter);
		tos(" testTime = ");
		ton(counter);
	}


	//pattern test

	tos("\n Bist test Pattern Test ");

	for(testCounter = 0; testCounter < 4;testCounter++){
		tos("\nPattern = ");
		ton(testCounter);
		tos(" writeTime = ");

		//fill with patterns
		*((unsigned int *)bistCache) = 0;//reset bist register
		counter = 0;
		bistCache->Bist_En = 1;
		bistCache->Pattern = testCounter;
		bistCache->Rd_Wr = 1;
		bistCache->RunBist = 1;
		while (bistCache->Done == 0){
			counter++;
		}
		ton(counter);

		passCounter = 0;

		//do one read pass
	//	while(passCounter < 50){
		counter = 0;
		bistCache->Bist_En = 0;
		bistCache->RunBist = 0;
		bistCache->Bist_En = 1;
		bistCache->Pattern = testCounter;
		bistCache->Rd_Wr = 0;
		bistCache->RunBist = 1;
		while (bistCache->Done == 0){
			counter++;
			if(bistCache->Error){
				bistCache->Error = 0;
				bistCache->RunBist = 0;
				bistCache->RunBist = 1;
				errorCounter++;
			}
		}

		tos(" readTime = ");
		ton(counter);
		tos(" errorCounter = ");
		ton(errorCounter);

		if(bistCache->Error){
			*((unsigned int *)bistCache) = 0;//reset bist register
			return bistCache->Error;
		}
	}
	*((unsigned int *)bistCache) = 0;//reset bist register
	passCounter++;
//	}
	return 0;
}


int bistTestMkio()
{
	int counter = 0;
	int errorCounter = 0;
	int passCounter = 0;
	int testCounter;
	int portCounter = 0;
	tos("\n =========== Bist test Mkio: ");

	BISTMKIO_UNIT * bistMkio = (BISTMKIO_UNIT*) bistMkioAddress;

	//test MARCH/MATS++

	tos("\n Bist test March Test ");
	for(portCounter=0;portCounter <2; portCounter++){
		tos("\n port =  ");
		ton(portCounter);
		for(testCounter = 0; testCounter < 4;testCounter++){
			counter=0;
			*((unsigned int *)bistMkio) = 0;
			bistMkio->Bist_En = 1;
			bistMkio->mode_test = 1;
			bistMkio->Alg = 1<<testCounter;
			bistMkio->N_port = portCounter;
			if(testCounter == 3)
				bistMkio->Alg = 7;

			bistMkio->RunBist = 1;

			while (bistMkio->Done == 0){
				counter++;
			}
			if(bistMkio->Error){
				*((unsigned int *)bistMkio) = 0;//reset bist register
				tos("\n Bist test failure! ");
				return bistMkio->Error;
			}
			tos("\n Pattern: ");
			ton(testCounter);
			tos(" testTime = ");
			ton(counter);
		}

	//pattern test

		tos("\n Bist test Pattern Test ");

		for(testCounter = 0; testCounter < 4;testCounter++){
			tos("\nPattern = ");
			ton(testCounter);
			tos(" writeTime = ");

			//fill with patterns
			*((unsigned int *)bistMkio) = 0;//reset bist register
			counter = 0;
			bistMkio->Bist_En = 1;
			bistMkio->N_port = portCounter;
			bistMkio->Pattern = 3-testCounter;
			bistMkio->Rd_Wr = 1;
			bistMkio->RunBist = 1;
			while (bistMkio->Done == 0){
				counter++;
			}
			ton(counter);

			passCounter = 0;

			//do one read pass
		//	while(passCounter < 50){
			counter = 0;
			bistMkio->Bist_En = 0;
			bistMkio->RunBist = 0;
			bistMkio->Bist_En = 1;
			bistMkio->N_port = portCounter;
			bistMkio->Pattern = 3-testCounter;
			bistMkio->Rd_Wr = 0;
			bistMkio->RunBist = 1;
			while (bistMkio->Done == 0){
				counter++;
				if(bistMkio->Error){
					bistMkio->Error = 0;
					bistMkio->RunBist = 0;
					bistMkio->RunBist = 1;
					errorCounter++;
				}
			}

			tos(" readTime = ");
			ton(counter);
			tos(" errorCounter = ");
			ton(errorCounter);

			if(bistMkio->Error){
				*((unsigned int *)bistMkio) = 0;//reset bist register
				return bistMkio->Error;
			}
		}

	}
	*((unsigned int *)bistMkio) = 0;//reset bist register
	passCounter++;
//	}
	return 0;
}



int bistTestSw()
{
	int counter = 0;
	int errorCounter = 0;
	int passCounter = 0;
	int testCounter;
	int portCounter = 0;
	tos("\n =========== Bist test Spw: ");

	BISTSW_UNIT * bistSw = (BISTMKIO_UNIT*) bistSwAddress;

	//test MARCH/MATS++

	tos("\n Bist test March Test ");
	for(portCounter=0;portCounter <2; portCounter++){
		tos("\n rxtx =  ");
		ton(portCounter);

		for(testCounter = 0; testCounter < 4;testCounter++){
			counter=0;
			*((unsigned int *)bistSw) = 0;
			bistSw->Bist_En = 1;
			bistSw->mode_test = 1;
			bistSw->Alg = 1<<testCounter;
			bistSw->rxtx = portCounter;
			if(testCounter == 3)
				bistSw->Alg = 7;

			bistSw->RunBist = 1;

			while (bistSw->Done == 0){
				counter++;
			}
			if(bistSw->Error){
				*((unsigned int *)bistSw) = 0;//reset bist register
				tos("\n Bist test failure! ");
				return bistSw->Error;
			}
			tos("\n Pattern: ");
			ton(testCounter);
			tos(" testTime = ");
			ton(counter);
		}

	//pattern test

		tos("\n Bist test Pattern Test ");

		for(testCounter = 0; testCounter < 4;testCounter++){
			tos("\nPattern = ");
			ton(testCounter);
			tos(" writeTime = ");

			//fill with patterns
			*((unsigned int *)bistSw) = 0;//reset bist register
			counter = 0;
			bistSw->Bist_En = 1;
			bistSw->rxtx = portCounter;
			bistSw->Pattern = 3-testCounter;
			bistSw->Rd_Wr = 1;
			bistSw->RunBist = 1;
			while (bistSw->Done == 0){
				counter++;
			}
			ton(counter);

			passCounter = 0;

			//do one read pass
		//	while(passCounter < 50){
			counter = 0;
			bistSw->Bist_En = 0;
			bistSw->RunBist = 0;
			bistSw->Bist_En = 1;
			bistSw->rxtx = portCounter;
			bistSw->Pattern = 3-testCounter;
			bistSw->Rd_Wr = 0;
			bistSw->RunBist = 1;
			while (bistSw->Done == 0){
				counter++;
				if(bistSw->Error){
					bistSw->Error = 0;
					bistSw->RunBist = 0;
					bistSw->RunBist = 1;
					errorCounter++;
				}
			}

			tos(" readTime = ");
			ton(counter);
			tos(" errorCounter = ");
			ton(errorCounter);

			if(bistSw->Error){
				*((unsigned int *)bistSw) = 0;//reset bist register
				return bistSw->Error;
			}
		}

	}
	*((unsigned int *)bistSw) = 0;//reset bist register

	passCounter++;
//	}
	return 0;
}



int bistTestId()
{
	unsigned int bistIdBuffer = *((unsigned int*)bistIdAddress) ;
	tos("\n =========== Bist test ID: ");
	ton(bistIdBuffer);
	if(bistIdBuffer  == bistId){
		tos(" success. ");
		return bistSuccess;
	}else{
		tos(" failure. Must be ");
		ton(bistId);
		return bistErrorId;
	}

}



int bistTest ()
{
	bistTestId();

	bistTestCache();
	bistTestMkio();
	bistTestSw();


}
