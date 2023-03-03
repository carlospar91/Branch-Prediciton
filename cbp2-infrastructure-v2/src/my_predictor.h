// my_predictor.h
// This file contains a sample gshare_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.

class gshare_update : public branch_update
{
public:
	unsigned int index;
};

class gshare_predictor : public branch_predictor
{
public:
#define HISTORY_LENGTH 15
#define TABLE_BITS 15

	gshare_update u;
	branch_info bi;
	unsigned int history;
	unsigned char tab[1 << TABLE_BITS];

	gshare_predictor(void) : history(0)
	{
		memset(tab, 0, sizeof(tab));
	}

	branch_update *predict(branch_info &b)
	{
		bi = b;
		if (b.br_flags & BR_CONDITIONAL)
		{
			u.index =
				(history << (TABLE_BITS - HISTORY_LENGTH)) ^ (b.address & ((1 << TABLE_BITS) - 1));
			u.direction_prediction(tab[u.index] >> 1);
		}
		else
		{
			u.direction_prediction(true);
		}
		u.target_prediction(0);
		return &u;
	}

	void update(branch_update *u, bool taken, unsigned int target)
	{
		if (bi.br_flags & BR_CONDITIONAL)
		{
			unsigned char *c = &tab[((gshare_update *)u)->index];
			if (taken)
			{
				if (*c < 3)
					(*c)++;
			}
			else
			{
				if (*c > 0)
					(*c)--;
			}
			history <<= 1;
			history |= taken;
			history &= (1 << HISTORY_LENGTH) - 1;
		}
	}
};

//
// Pentium M hybrid branch predictors
// This class implements a simple hybrid branch predictor based on the Pentium M branch outcome prediction units.
// Instead of implementing the complete Pentium M branch outcome predictors, the class below implements a hybrid
// predictor that combines a bimodal predictor and a global predictor.
class pm_update : public branch_update
{
// Gshare example  
// #define HISTORY_LENGTH 15
// #define TABLE_BITS 15

// 	gshare_update u;
// 	branch_info bi;
// 	unsigned int history;
// unsigned char tab[1 << TABLE_BITS];
public:
	//unsigned int index;
	unsigned int bimodal_Iindex;
	unsigned int global_Index;
};

class pm_predictor : public branch_predictor
{
public:
#define HISTORY_LENGTH 15
#define TABLE_BITS 15

	pm_update u;
	branch_info bi;
	unsigned int history;
	unsigned char bimodalTable[1 << TABLE_BITS];
	unsigned char globalTable[1 << TABLE_BITS];

	pm_predictor(void)
	{
		// fill bimodal table and global predictor
		memset(bimodalTable, 0, sizeof(bimodalTable));
		memset(globalTable, 0, sizeof(globalTable));
	}

	branch_update *predict(branch_info &b)
	{
		bi = b;

		if (b.br_flags & BR_CONDITIONAL)
		{
			// Gshare Example global History XOR branch address
			// u.index =
			// 	(history << (TABLE_BITS - HISTORY_LENGTH)) ^ (b.address & ((1 << TABLE_BITS) - 1));
			
			//Create both 
			//Bimodal 
			u.bimodal_Iindex = 
				(b.address & ((1 << TABLE_BITS) - 1));
			//Global 
			u.global_Index = 
				(history << (TABLE_BITS - HISTORY_LENGTH));//from Class this just works 

			//next we decice which We will look at the right most bit to
			// the final prediction will be determined by selecting the prediction from the global predictor. Appendix B  
			// u.direction_prediction(tab[u.index] >> 1);

			//
			int rightMostBit = (history >> 0) & 1;
			if(rightMostBit == 1)//this is my attempt to choose a path. I picked the right most bit and if it hits then choose the global prediction
			{
				u.direction_prediction(globalTable[u.global_Index] >> 1);
				
			} else 
			{	
				u.direction_prediction(bimodalTable[u.bimodal_Iindex] >> 1);
			}
		//Gshare example 
		// } else
		// {
		// 	u.direction_prediction(true);
		// }

		// u.target_prediction(0);
		// return &u;

		} else
		{
			u.direction_prediction(true);
		}

		u.target_prediction(0);
		return &u;
	}

	void update(branch_update *u, bool taken, unsigned int target)
	{

		if (bi.br_flags & BR_CONDITIONAL)
		{
			// Gshare example 
			// unsigned char *c = &tab[((gshare_update *)u)->index];
			unsigned char *bmodal = &bimodalTable[((pm_update *)u)->bimodal_Iindex];
			unsigned char *glo = &globalTable[((pm_update *)u)->global_Index];


			// if (taken)
			// {
			// 	if (*c < 3)
			// 		(*c)++;
			// }
			// else
			// {
			// 	if (*c > 0)
			// 		(*c)--;
			// }
			

			if (taken)
			{
				if (*glo < 3)
				{
					(*glo)++;
				}
				if (*bmodal < 3)
				{	
					(*bmodal)++;
				}
			}
			else
			{

				if (*glo > 0)
				{
					(*glo)--;
				}
				if (*bmodal > 0) 
				{
					(*bmodal)--;
				}
			}


			// history <<= 1;
			// history |= taken;
			// history &= (1 << HISTORY_LENGTH) - 1;
			history <<= 1;
			history |= taken;
			history &= (1 << HISTORY_LENGTH) - 1;
			
		}
	}
};

//
// Complete Pentium M branch predictors for extra credit
// This class implements the complete Pentium M branch prediction units.
// It implements both branch target prediction and branch outcome predicton.
class cpm_update : public branch_update
{
public:
	unsigned int index;
};

class cpm_predictor : public branch_predictor
{
public:
	cpm_update u;

	cpm_predictor(void)
	{
	}

	branch_update *predict(branch_info &b)
	{
		u.direction_prediction(true);
		u.target_prediction(0);
		return &u;
	}

	void update(branch_update *u, bool taken, unsigned int target)
	{
	}
};
