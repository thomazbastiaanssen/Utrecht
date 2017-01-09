#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <grace_np.h>
#include <unistd.h>
#include <float.h>
#include <limits.h>
#include <signal.h>
#include <cash2003.h>
#include <cash2.h>
#include <mersenne.h>
#include <cash2-s.h>


static TYPE2** Mut;
static TYPE2** ColorAntHelp;
static TYPE2** ColorPlantHelp;
static TYPE2** ColorTrampHelp;
static TYPE2** ColorMap;


void Initial(void)
{
    MaxTime = 2147483647; /* default=2147483647 */
    nrow = 100; /* # of row (default=100)*/
    ncol = 100; /* # of column (default=100)*/
    nplane = 5; /* # of planes (default=0)*/
    scale = 2; /* size of the window (default=2)*/
    boundary = WRAP; /* the type of boundary: FIXED, WRAP, ECHO (default=WRAP). Note that
    	      Margolus diffusion is not supported for ECHO. */
    ulseedG = 10203; /* random seed (default=56)*/

    /* useally, one does not have to change the followings */
    /* the value of boundary (default=(TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.})*/
    boundaryvalue2 = (TYPE2){0,0,0,0,0,0.,0.,0.,0.,0.};
}

TYPE2 Plant, Ant, Tramp, Empty;
void InitialPlane(void)
{


    //MakePlane(&Mut);
    MakePlane(&Mut,&ColorAntHelp,&ColorPlantHelp,&ColorTrampHelp,&ColorMap); // Init all planes
    Boundaries2(Mut);
    Boundaries2(ColorAntHelp);
    Boundaries2(ColorPlantHelp);
    Boundaries2(ColorTrampHelp);
    Boundaries2(ColorMap);

    Plant.val = 1;
    Plant.fval = .4; //birth rate
    Plant.fval2 = .7; //how much the plant is helped by the ant
    Plant.fval3 = 0.2; //deathrate


    Ant.val = 2;
    Ant.fval = .4; //birth rate
    Ant.fval2 = .7; //how much the ant is helped by the plant
    Ant.fval3 = 0.2; //deathrate

    Tramp.val = 3;
    Tramp.fval = .2; //birth rate
    Tramp.fval2 = .7; //how much the tramp is helped by the plant
    Tramp.fval3 = 0.2; //deathrate


    InitialSetS(Mut,3,Empty,
        Plant, 0.1,
        Ant,   0.1,
        Tramp, 0.1) ;

    int i,j;
    for(i=1;i<=nrow;i++)for(j=1;j<=ncol;j++)
    {
        if(j>10 && j < 30)
            ColorMap[i][j].val= 110 - (float)i/(float)nrow*100;	//maps fval on Comp plane to val on Color plane
    }

    // Initialise colorRGB table (for colouring 2nd and 3rd plane)

    int r=0,g=0,b=255;
    double nr=102.;    //nr of ColorRGB's you want to create
    double range=1275.;  // range of coloursteps: 255*5= 1275
    double x = range/nr;
    int y=0,c;

    for(c=0;c<(int)range;c++){
        if(c<255){			//starts blue
          r = r + 1;		//goes magenta
        }else if(c<255*2){
          b = b - 1;		//goes red
        }else if(c<255*3){
          g = g + 1;		//goes yellow
        }else if(c<255*4){
          r = r -1;    		//goes green
        }else if(c<255*5){
          b = b + 1;		//goes cyan
        }

        if(c == (int)(y*x+0.5)){
          ColorRGB(10+y,r,g,b);	//so colour indexes in the gradient start at 10
          y++;
        }
    }

    ColorRGB(1,0,200,0);
    ColorRGB(2,0,150,255);
    ColorRGB(3,255,128,255);
}





void NextState(int row,int col)
{
    //NextStateRM(row,col);
    NextStateComp(row,col);

}

float mut = .1; float mutstep = .1;
TYPE2 Mutate (TYPE2 mutateme){
    if (genrand_real1() < mut) { // do mutations
        mutateme.fval2 += mutstep*(genrand_real1()-.5);

        //bounds of mutated parameters
        mutateme.fval2 = fmaxf(0.,mutateme.fval2);
        mutateme.fval2 = fminf(1.,mutateme.fval2);
    }
    return mutateme;
}

float inv = .95;
float CheckRepr (TYPE2 rn, int rn_row, int rn_col)
{
    if (rn.val == Plant.val){
        int rn_ants = CountMoore8(Mut, Ant.val, rn_row,rn_col);
        return fminf(inv, /*rn.fval + */rn.fval2*rn_ants);
    }
    else {if (rn.val == Ant.val || rn.val == Tramp.val ){
        int rn_plants = CountMoore8(Mut, Plant.val, rn_row,rn_col);
        return fminf(inv, /*rn.fval + */rn.fval2*rn_plants);
    }}
    return 0.0;
}

void NextStateRM(int row, int col)
{

    TYPE2 *me = &Mut[row][col]; // for easy reference

    if (me->val == Empty.val)
    {
        int rn_col, rn_row; // location of random neighbour
        RandomMooreC8(Mut,row,col,&rn_row,&rn_col);
        TYPE2 rn = Mut[rn_row][rn_col];

        rn = Mutate(rn);

        if (genrand_real1() < CheckRepr(rn, rn_row, rn_col))
            *me = rn;
    }
    if (genrand_real1() < me->fval3) *me = Empty; //deathrate
}


float dth = .2; //Death rate
void NextStateComp(int row, int col)
{
    TYPE2 *me = &Mut[row][col]; // for easy reference

    int neirow, neicol;
    float scores[8];
    float totscore = 0;
    if(me->val == 0){
        for(int direction = 0; direction < 8; direction++ ){
            GetNeighborC(Mut,row,col,direction+1,&neirow,&neicol);
            TYPE2 nei = Mut[neirow][neicol];

            scores[direction] = CheckRepr(nei, neirow, neicol); //Plant.fval+Ant.fval2*(CountMoore8(Mut,2,neirow,neicol));
            totscore += scores[direction];
        }
        if(totscore == 0){ //nothing is around me and I am empty :(
        Mut[row][col].val = Empty.val;
        }
        else{
            if (genrand_real1() > inv) return; //not 100% invasion

            float roll = genrand_real1();
            float rest = 0;
            for(int i = 0 ; i < 8 ; i ++){
                if(roll < (scores[i]/totscore) + rest){
                    *me = Mutate(GetNeighborS(Mut, row, col, i+1));
                    break;
                }
                else{
                    rest += (scores[i]/totscore);
                }
            }
        }

    }
    if (genrand_real1() < me->fval3) *me = Empty; //deathrate
}

double max_fval=1.;		// Upper bound (if applicable) for your evolvable parameter
// This function maps fvals stored in individuals to a colorvalue in the colorRGB table
int GetColorIndexFrom(int val,double fval, int me)
{
    int color;
    double max_fval_we_color;

    if (val != me) return 0;

    if(val==0) return 0;
    max_fval_we_color = (max_fval*100. - 10.)/100.; //Remember that colour indexes in the gradient start at 10
    if(fval> max_fval_we_color ) color=100;
    else color= (int)( 100.* fval/max_fval )+10 ;
    return color;
}

void Update(void)
{
    Display(Mut,ColorAntHelp,ColorPlantHelp,ColorTrampHelp,ColorMap);
    Synchronous(1,Mut);



    // ************************************************************************************//
    // GRAPHING Below is some code to plot fvals...
    	// 1) ...over time for 120 random individuals every 10 timesteps
    	// 2) ...as a color in the second plane
    // ************************************************************************************//
      int i, j;
    if(Time%10==0) {

       int r;
      for(r=1;r<=120;r++)
    	{
              i = genrand_int(1,nrow);
    	  j = genrand_int(1,ncol);
    	  if(Mut[i][j].val > 0)
                PlotXY(Time,Mut[i][j].fval2);
    	}
      // Setup display of graph (once)

    }


    if(Time == 0)
    {
    	GracePrintf("g0 on");
    	GracePrintf("with g0");
    	GracePrintf("focus g0");
    	GracePrintf("s1 on");
    	GracePrintf("s1 color 5");
    	GracePrintf("s1 line linestyle 0");
    	GracePrintf("s1 symbol 1");
    	GracePrintf("s1 symbol size 0.2");
    }

    for(i=1;i<=nrow;i++)for(j=1;j<=ncol;j++){
        ColorAntHelp[i][j].val=GetColorIndexFrom(Mut[i][j].val, Mut[i][j].fval2, Ant.val);	//maps fval on Mut plane to val on Color plane
        ColorPlantHelp[i][j].val=GetColorIndexFrom(Mut[i][j].val, Mut[i][j].fval2, Plant.val );	//maps fval on Mut plane to val on Color plane
        ColorTrampHelp[i][j].val=GetColorIndexFrom(Mut[i][j].val, Mut[i][j].fval2, Tramp.val );
    }

    // ************************************************************************************//
    // END PLOTTING PART
    // ************************************************************************************//

}
