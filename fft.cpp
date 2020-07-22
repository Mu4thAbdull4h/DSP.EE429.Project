

#include <complex>
#include <math.h>
//#include <fstream>
#include <stdlib.h>     
#include <graphics.h>
#include <winbgim.h>
#include <string>
#include <conio.h>
#include <sstream> 

#define DataLength			1024
#define GraphTBColor		0x555
#define GraphTColor			GraphTBColor+0x111111
#define GraphFBColor		0x555000
#define GraphFColor			GraphFBColor+0x111111
#define PI					3.141592653589793238460
 
typedef std::complex<double> Complex;

using namespace std;
int rotationStep = 10;

enum 
	{ 	
		SINE,
		COSINE,
		PULSE,
		SINC,
		RAMP,
		SQUARE,
		EXP
	};
	
enum 	
	{  
		REAL = 1,
		IMAG,
		MAGNITUDE,
		PHASE
	};

class Button
	{
		private : 
			unsigned int		
				xStart 	,
				yStart 	,
				Width  	,
				High  	,
				BColor	,
				FColor	;						
			string 
				Label ;   
	 
		public  : 
		
		Button()
		{
			xStart	= 500;
			yStart	= 500;
			Width 	= 200;
			High 	= 30;
			Label 	= "Button";
			BColor	= 0xffffff;
			FColor	= 0x0;	
			CreateButton();
		}		
	 
		Button(int xStart,int yStart,int Width,int High,string Label,int BColor,int FColor)
		{
			this->xStart	= xStart;
			this->yStart	= yStart;
			this->Width 	= Width;
			this->High 		= High;
			this->Label 	= Label;
			this->BColor	= BColor;
			this->FColor	= FColor;	
			CreateButton();
		}		

		
		
		void setLabel(string Label)
			{
				this->Label = Label;
				CreateButton();
			}
			
		bool isClicked(int x, int y)
			{
				return (x>xStart & x<xStart+Width)&&(y>yStart & y<yStart+High);
			}

		void setBColor(int C)
			{
				 this->BColor = C;
			}
			
		int getBColor()
			{
				return BColor;
			}
			
		void setPosition(int xStart,int yStart)
			{
				this->xStart 	= xStart;
				this->yStart 	= yStart;
				CreateButton();
			}
	 
		void getPosition(int * x , int * y)
			{
				x = (int*)xStart;
				y = (int*)yStart;
			}		
		
		void CreateButton()
			{
				setcolor(FColor);
				setbkcolor(BColor);
				rectangle(xStart,yStart,xStart+Width,yStart+High);
				setfillstyle	(1,BColor);   
				floodfill		(xStart+1,yStart+1,FColor);		 
				outtextxy(xStart+Width/2-Label.length()*3-1,yStart+6,(char*)Label.c_str());		 
			}
		
	};

void Filter(Complex *x,int FStart,int FEnd);
void RoR(Complex *x);
void RoL(Complex *x);
void fft(Complex * x);
void Normalize(Complex * x); 
void shiftfft(Complex * x);
void ifft(Complex * x);


inline void zeros(Complex *x)
			{
				for (int c = 0 ; c < DataLength ; c++)
				{
					x[c] = 0;				 
				}
		    }



int main(int argc, char** argv){

    initwindow(1355,700,(char*)"Time Domain to Frequency Domain Using FFT [ EE429 Project (DSP) - St.MAlyoubi - Dr.ABalamash - ECE Department - Faculty of Engineering - King AbdulAziz University - Feb 2020 ]",200,200,false,false);
	
	int WDTH 				= getmaxx();
	int HIGH 				= getmaxy();	
	int MousePositionX		= 0;
	int MousePositionY		= 0;
	int	TOTAL_SCREEN_W    	= 1024, 
		TOTAL_SCREEN_H    	= HIGH,
		HALF_HIGH_LINE    	= TOTAL_SCREEN_H/2+1,
		HALF_LINE_GRAPH   	= HALF_HIGH_LINE/2,
		HALF_WDTH_LINE    	= TOTAL_SCREEN_W/2,
		VERTICAL_FGRAPH   	= TOTAL_SCREEN_H-1,
		START_FFT_GRAPH_H 	= HALF_LINE_GRAPH+HALF_HIGH_LINE,
		FFT_RESOLUTION    	= HALF_HIGH_LINE/2-2,
		SCREEN_WEDTH_GRAPH	= TOTAL_SCREEN_W-2,
		FFT_RESOLUTION_Ph 	= (int)(FFT_RESOLUTION/3.1415),
		FFT_RESOLUTION_Abs	= (int)(FFT_RESOLUTION*0.7071),
			XposStart 		= 1,
			YposStart 		= 1,
			XposEnd   		= 1,
			YposEnd   		= 1,
			TypeOfGraphFlag = 4,
			DrawModeRegion  = 1,
			MoveMouseBwF_and_Time = 0;
			
 	bool    drawflg		 	= true,
			DrawModeFlag  	= true,
			ClearTheData  	= false;
			
 	double 	slope 			= 1;
	char 	Char 			= ' ';
	
	Complex Data[DataLength]= {0,0,100,100,100,0};
	Complex Pure[DataLength]= {};
    string 	CoordinatesStr;	
	
	setbkcolor		(0xaaaaaa); 
	clearviewport	(); 
    setcolor		(WHITE);    
	setbkcolor		(GraphTBColor); 
	rectangle		(0,0,TOTAL_SCREEN_W,TOTAL_SCREEN_H);
    line			(0,HALF_HIGH_LINE,TOTAL_SCREEN_W,HALF_HIGH_LINE);    
	setfillstyle	(7,GraphTColor);   
    floodfill		(1,1,WHITE);
    setcolor		(GraphTColor+0x333333);
    line			(1,HALF_LINE_GRAPH,TOTAL_SCREEN_W-1,HALF_LINE_GRAPH);
    setbkcolor		(GraphFBColor);
   	setfillstyle	(7,GraphFColor);   
    floodfill		(1,HALF_HIGH_LINE+1,WHITE);
    setcolor		(GraphFColor+0x333333);
    line			(1,HALF_LINE_GRAPH+HALF_HIGH_LINE-2,TOTAL_SCREEN_W-1,HALF_LINE_GRAPH+HALF_HIGH_LINE-2);   
    line			(HALF_WDTH_LINE,HALF_HIGH_LINE+1,HALF_WDTH_LINE,VERTICAL_FGRAPH);
    
    int FreqParameter = 1,
		DrawInTimeOrF = 0;
    
    Button fftShiftButton(TOTAL_SCREEN_W+5,60,WDTH-TOTAL_SCREEN_W-7,30," FFT Shift is Enabled ",0xaa0000,0xFFFFFF);
    
    Button SineFunction(TOTAL_SCREEN_W+5,180,WDTH-TOTAL_SCREEN_W-175,30,"  Sine Function  ",0xaa0000,0xFFFFFF);
    Button IncFSine(TOTAL_SCREEN_W+165,180,80,30,"IncFreq",0xff,0xFFFFFF);
    Button DecFSine(TOTAL_SCREEN_W+247,180,80,30,"DecFreq ",0xff,0xFFFFFF);    
    
    Button CosineFunction(TOTAL_SCREEN_W+5,220,WDTH-TOTAL_SCREEN_W-175,30,"  Cos Function  ",0xaa0000,0xFFFFFF);
    Button IncFCosine(TOTAL_SCREEN_W+165,220,80,30,"IncFreq",0xff,0xFFFFFF);
    Button DecFCosine(TOTAL_SCREEN_W+247,220,80,30,"DecFreq ",0xff,0xFFFFFF);  
    
    Button pulse(TOTAL_SCREEN_W+5,260,80,30,"Pulse",0x5500,0xFFFFFF);  
	Button sinc(TOTAL_SCREEN_W+86,260,80,30,"Sinc",0x5500,0xFFFFFF);
	//Button sincInc(TOTAL_SCREEN_W+170,260,75,30,"Expan",0x5500,0xFFFFFF);
	Button expan(TOTAL_SCREEN_W+246,260,75,30,"Expan",0x5500,0xFFFFFF);
	Button EXIT(TOTAL_SCREEN_W+5,HIGH-40,WDTH-TOTAL_SCREEN_W-7,30,"EXIT",0x99,0xFFFFFF);
	
	
	
    
    bool   	FlagScreenClick = false,
		    FlagFFTshift	= true;	
		     
	int FirstMouseClick = 0,
		FinalMouseClick = 0;
    

 	
 	 while (1)
		{
			if(DrawInTimeOrF)
			{
				if(ismouseclick(WM_LBUTTONDOWN))
				{
					FinalMouseClick = mousex();
					if(FinalMouseClick>FirstMouseClick) 
						{
							Filter(Data,FirstMouseClick,FinalMouseClick);	
						}
					else if(FinalMouseClick<FirstMouseClick) 
					{
						Filter(Data,FinalMouseClick,FirstMouseClick);
					}
					for(int c = 0 ; c < DataLength;c++)
					{
						Pure[c] = Data[c];
					}
							
					if(FlagFFTshift)
					{
						shiftfft(Pure);	
					}
						
					ifft(Pure);
					Normalize(Pure);
					for(int c = 0 ; c < DataLength;c++)
					{
						Pure[c] = Complex(Pure[c].real()*FFT_RESOLUTION,Pure[c].imag()*FFT_RESOLUTION);
					}
						
					clearmouseclick(WM_LBUTTONDOWN);  
					DrawInTimeOrF=false;
					drawflg   = true; 
				}    
			}
			else if(FlagScreenClick)
			{
				if(ClearTheData)
				{
					 zeros(Data);				    
					 clearmouseclick(WM_MOUSEWHEEL);
					 ClearTheData = false;
				}								 
				XposEnd = mousex();
				YposEnd = mousey();  
				double slope = 1;
					 
				if(XposEnd < TOTAL_SCREEN_W and YposEnd < HALF_HIGH_LINE)
				{
					if((XposEnd-XposStart))
					{
						slope = (double)(YposEnd-YposStart)/(XposEnd-XposStart);	
					} 
					switch(DrawModeRegion)
					{
					case REAL:
						for(int i = XposStart; i < XposEnd ; i++)
						{								
							Data[i]=Complex(-1*((YposStart)-HALF_HIGH_LINE/2),0.0);
							YposStart+=(int)slope;
						}
						break;
					case IMAG:
						for(int i = XposStart; i < XposEnd ; i++)
						{								
							Data[i]= Complex(0.0,-1*((YposStart)-HALF_HIGH_LINE/2));
							YposStart+=(int)slope;
						} 
						break;
					case MAGNITUDE:
						for(int i = XposStart; i < XposEnd ; i++)
						{								
							Data[i]= Complex(0.0,-1*((YposStart)-HALF_HIGH_LINE/2));
							YposStart+=(int)slope;
						} 
						break;
					case PHASE:
						for(int i = XposStart; i < XposEnd ; i++)
						{ 
							double v 	= -1*((double)(YposStart)-HALF_HIGH_LINE/2);
							Data[i]		= Complex((double)cos(v*PI/180)*FFT_RESOLUTION_Ph,sin((double)v*PI/180)*FFT_RESOLUTION_Ph);
							YposStart  += (int)(double)(YposEnd-YposStart)/(XposEnd-XposStart);																
						}
						break;
					}
				}
				if(XposStart != XposEnd | YposStart != YposEnd)
				{
					setcolor(WHITE);    
					setbkcolor(GraphTBColor); 
					rectangle(0,0,TOTAL_SCREEN_W,TOTAL_SCREEN_H);
					line(0,HALF_HIGH_LINE,TOTAL_SCREEN_W,HALF_HIGH_LINE);    
					setfillstyle(7,GraphTColor);   
					floodfill(1,1,WHITE);
					setcolor(GraphTColor+0x333333);  
					setcolor( 0xff );				  
					outtextxy(2,1,"0");
					outtextxy(TOTAL_SCREEN_W-35,1,"1024"); 
					//outtextxy(TOTAL_SCREEN_W-90,1,"Time Domain");
					line(1,HALF_LINE_GRAPH,TOTAL_SCREEN_W-1,HALF_LINE_GRAPH); 
					
					setbkcolor(GraphFBColor); 
					
					if(DrawModeFlag)
					{
						setbkcolor( 0xaaaaaa );
						setcolor( 0x000 );
						outtextxy(TOTAL_SCREEN_W+5,25,"[+] DrawingMode : ");
						setcolor( 0xff );
						outtextxy(TOTAL_SCREEN_W+130,25,"ON    ");
						DrawModeFlag=false;
					}
							 
					switch(DrawModeRegion)
					{				
						case REAL :  	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(Data[x].real())),x+1,(int)(HALF_LINE_GRAPH - (int)(Data[x+1].real()))); 					break;
						case IMAG :  	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(Data[x].imag())),x+1,(int)(HALF_LINE_GRAPH - (int)(Data[x+1].imag()))); 					break;     
						case MAGNITUDE:	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(abs(Data[x]))),  x+1,(int)(HALF_LINE_GRAPH - (int)(abs(Data[x+1]))));						break;
						case PHASE:		for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(arg(Data[x])*180/PI)),x+1,(int)(HALF_LINE_GRAPH - (int)(arg(Data[x+1])*180/PI)));			break;
					}
				}
				if(XposEnd < TOTAL_SCREEN_W and YposEnd < HALF_HIGH_LINE)
				{
					YposStart = YposEnd;  
					XposStart = XposEnd; 
				}
			  
			 }
			else if(drawflg)
			{     
				
				setcolor(WHITE);    
				setbkcolor(GraphTBColor); 
				rectangle(0,0,TOTAL_SCREEN_W,TOTAL_SCREEN_H);
				line(0,HALF_HIGH_LINE,TOTAL_SCREEN_W,HALF_HIGH_LINE);    
				setfillstyle(7,GraphTColor);				 
				floodfill(1,1,WHITE);
				setcolor(GraphTColor+0x333333);
				line(1,HALF_LINE_GRAPH,TOTAL_SCREEN_W-1,HALF_LINE_GRAPH);
				switch(DrawModeRegion)
				{
					case REAL 		: outtextxy(TOTAL_SCREEN_W/2-10,2,"Real"); 		break; 
					case IMAG 		: outtextxy(TOTAL_SCREEN_W/2-25,2,"Imaginary"); break;
					case MAGNITUDE 	: outtextxy(TOTAL_SCREEN_W/2-25,2,"Magnitude"); break;
					case PHASE 		: outtextxy(TOTAL_SCREEN_W/2-10,2,"Phase"); 	break;
				}
				
				setcolor( 0xff );
				setbkcolor(GraphFBColor);   
				

				switch(DrawModeRegion)
				{				
					case REAL :  	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(Pure[x].real())),x+1,(int)(HALF_LINE_GRAPH - (int)(Pure[x+1].real()))); 										break;
					case IMAG :  	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(Pure[x].imag())),x+1,(int)(HALF_LINE_GRAPH - (int)(Pure[x+1].imag()))); 										break;     
					case MAGNITUDE:	for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(abs(Pure[x]))),x+1,(int)(HALF_LINE_GRAPH - (int)(abs(Pure[x+1]))));											break;
					case PHASE:		for (int x = 0 ; x < DataLength ; x+=1) line(x,(int)(HALF_LINE_GRAPH - (int)(arg(Pure[x])*FFT_RESOLUTION_Ph)),x+1,(int)(HALF_LINE_GRAPH - (int)(arg(Pure[x+1])*FFT_RESOLUTION_Ph)));		break;
				}
				
			  
				setbkcolor( 0xaaaaaa );
				setbkcolor(GraphFBColor);
				setfillstyle(7,GraphFColor);   
				floodfill(1,HALF_HIGH_LINE+1,WHITE);
				setcolor(GraphFColor+0x333333);
				line(1,HALF_LINE_GRAPH+HALF_HIGH_LINE-2,TOTAL_SCREEN_W-1,HALF_LINE_GRAPH+HALF_HIGH_LINE-2);   
				line(HALF_WDTH_LINE,HALF_HIGH_LINE+1,HALF_WDTH_LINE,VERTICAL_FGRAPH);
				setcolor( 0x00ff00 );   
				outtextxy(TOTAL_SCREEN_W-125,HALF_HIGH_LINE+1,"Frequency Domain"); 

				switch(TypeOfGraphFlag)
				{
					case 1 : 
						for (int x = 0 ; x < DataLength ; x++)  
						{
							line(x,(int)(START_FFT_GRAPH_H - (int)(Data[x].real()*FFT_RESOLUTION)),x+1,(int)(START_FFT_GRAPH_H - (int)(Data[x+1].real()*FFT_RESOLUTION)));								 
						}
						outtextxy(1,HALF_HIGH_LINE+1,"Real Part"); 
						break;
					case 2 : 
						for (int x = 0 ; x < DataLength ; x++)  
						{
							line(x,(int)(START_FFT_GRAPH_H - (int)(Data[x].imag()*FFT_RESOLUTION)),x+1,(int)(START_FFT_GRAPH_H - (int)(Data[x+1].imag()*FFT_RESOLUTION))); 
						}
						outtextxy(1,HALF_HIGH_LINE+1,"Imaginary Part"); 
						break;
					case 3 : 
						for (int x = 0 ; x < DataLength ; x++)  
						{
							line(x,(int)(START_FFT_GRAPH_H - (int)(arg(Data[x])*FFT_RESOLUTION_Ph)),x+1,(int)(START_FFT_GRAPH_H - (int)(arg(Data[x+1])*FFT_RESOLUTION_Ph)));
						}
						outtextxy(1,HALF_HIGH_LINE+1,"Phase Angle"); 
						break;
					case 4 : 
						for (int x = 0 ; x < DataLength ; x++)  
						{
							line(x,(int)(START_FFT_GRAPH_H - (int)(abs(Data[x])*FFT_RESOLUTION_Abs)),x+1,(int)(START_FFT_GRAPH_H - (int)(abs(Data[x+1])*FFT_RESOLUTION_Abs))); 
						}
						outtextxy(1,HALF_HIGH_LINE+1,"Magnitude"); 
						break;		   
				} 
				 
				if(FlagFFTshift)
				{
					 setcolor( 0x00ffff);
					 outtextxy(TOTAL_SCREEN_W-8,HALF_HIGH_LINE+FFT_RESOLUTION-7,"+"); 
					 outtextxy(TOTAL_SCREEN_W/2-3,HALF_HIGH_LINE+FFT_RESOLUTION-7,"0"); 
					 outtextxy(2,HALF_HIGH_LINE+FFT_RESOLUTION-7,"-");				
				}								 
				drawflg=false;
			}
			else
			{
				if(!(GetKeyState(VK_SHIFT) & 0x8000))
				{
					if(ismouseclick(WM_LBUTTONDOWN))
					{
						getmouseclick(WM_LBUTTONDOWN,MousePositionX,MousePositionY);
						
						if(MousePositionX< TOTAL_SCREEN_W and MousePositionY < HALF_HIGH_LINE)
						{
							FlagScreenClick = true;
						}
						else if(MousePositionX< TOTAL_SCREEN_W and MousePositionY > HALF_HIGH_LINE)
						{
							DrawInTimeOrF 	= true;
							FirstMouseClick = mousex();
						}
						else if (fftShiftButton.isClicked(MousePositionX,MousePositionY))
						{
							FlagFFTshift = !FlagFFTshift;
							fftShiftButton.setBColor(fftShiftButton.getBColor() xor 0xaa00bb);
							
							if(FlagFFTshift)
							{
								fftShiftButton.setLabel(" FFT Shift is Enabled ");
							}
							else 
							{
								fftShiftButton.setLabel(" FFT Shift is Disabled ");
							}
							shiftfft(Data);
							drawflg = true;
						}
						else if(EXIT.isClicked(MousePositionX,MousePositionY))
						{
							closegraph(); 
							exit(0);
						}

						else if(SineFunction.isClicked(MousePositionX,MousePositionY))
						{
							
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}
						else if(CosineFunction.isClicked(MousePositionX,MousePositionY))
						{						
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}
						else if(DecFSine.isClicked(MousePositionX,MousePositionY))
						{	
							if(FreqParameter<40)
							{
								FreqParameter+=3;				
							}
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}		 					
						else if(IncFSine.isClicked(MousePositionX,MousePositionY))
						{	
							if(FreqParameter>1)
							{
								FreqParameter-=3;					
							}
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = sin((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						} 
						else if(DecFCosine.isClicked(MousePositionX,MousePositionY))
						{						  
							if(FreqParameter<40)FreqParameter+=3;					
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}  
						else if(IncFCosine.isClicked(MousePositionX,MousePositionY))
						{						  
							if(FreqParameter>1)
							{
								FreqParameter-=3;					
							}
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = cos((double)2*c/FreqParameter)*FFT_RESOLUTION;
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}
						else if(pulse.isClicked(MousePositionX,MousePositionY))
						{						  
												
							for(int c = 0 ; c < FreqParameter;c++)
							{								 
								Data[c] = 1*FFT_RESOLUTION;
								Pure[c] = 1*FFT_RESOLUTION;								    
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}
						else if(sinc.isClicked(MousePositionX,MousePositionY))
						{						  
							double angle = -(DataLength/2)+0.1	;				
							for(int c = 0; c<DataLength;c++)
							{
								Data[c] = sin(angle/FreqParameter)/(angle/FreqParameter)*FFT_RESOLUTION;
								Pure[c] = sin(angle/FreqParameter)/(angle/FreqParameter)*FFT_RESOLUTION; 
								angle++  ;								
							}		    
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true; 
						}
							// rotationStep
					}
					MoveMouseBwF_and_Time = 0 ;
				}
				else
				{
					if(mousey()<HALF_HIGH_LINE)
					{
						XposEnd = mousex(); 
						if(MoveMouseBwF_and_Time != 1) 
						{ 
							XposEnd = XposStart; 
							MoveMouseBwF_and_Time = 1; 
						}
						rotationStep = XposEnd - XposStart;						
						if(rotationStep>0)
						{
							RoR(Pure);
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = Pure[c];
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true;
						}
						else if(rotationStep < 0 and rotationStep>-1024)
						{
							rotationStep = XposStart-XposEnd;								
							RoL(Pure);
							for(int c = 0 ; c < DataLength;c++)
							{
								Data[c] = Pure[c];
							}
							fft(Data); 
							Normalize(Data);
							if(FlagFFTshift)
							{
								shiftfft(Data);
							}
							drawflg   = true;
						}
					}
					else
					{
						XposEnd = mousex();   
						if(MoveMouseBwF_and_Time != 2) 
						{ 
							XposEnd = XposStart; MoveMouseBwF_and_Time = 2; 
						}
						rotationStep = XposEnd - XposStart;
								
						if(rotationStep>0)
						{
							RoR(Data);
							for(int c = 0 ; c < DataLength;c++)
							{
								Pure[c] = Data[c];
							}
							if(FlagFFTshift)
							{
								shiftfft(Pure);
							}
							ifft(Pure); 
							Normalize(Pure);	
							for(int c = 0 ; c < DataLength;c++)
							{
								Pure[c] = Complex(Pure[c].real()*FFT_RESOLUTION,Pure[c].imag()*FFT_RESOLUTION);									
							}
							drawflg = true;
						}
						else if(rotationStep < 0 and rotationStep>-1024)
						{
							rotationStep = XposStart-XposEnd;								
							RoL(Data);
							for(int c = 0 ; c < DataLength;c++)
							{
								Pure[c] = Data[c];
							}
							if(FlagFFTshift)
							{
								shiftfft(Pure);
							}
							ifft(Pure); 
							Normalize(Pure);				
							for(int c = 0 ; c < DataLength;c++)
							{
								Pure[c] = Complex(Pure[c].real()*FFT_RESOLUTION,Pure[c].imag()*FFT_RESOLUTION);								
							}
							drawflg = true;
						}						
					}
					clearmouseclick(WM_LBUTTONDOWN);
					XposStart = XposEnd;				
				}
			}
				
					
			if(ismouseclick(WM_RBUTTONDOWN)) 
			{
				getmouseclick(WM_RBUTTONDOWN,MousePositionX,MousePositionY);
				if(FlagScreenClick)
				{				
					setbkcolor(0xaaaaaa);
					setcolor( 0x0 );
					outtextxy(TOTAL_SCREEN_W+5,25,"[+] DrawingMode : ");
					outtextxy(TOTAL_SCREEN_W+130,25,"OFF    ");
					for(int c = 0 ; c < DataLength;c++)
					{
						Pure[c] = Data[c];
					}
					fft(Data);				 
					Normalize(Data);
					if(FlagFFTshift)
					{
						shiftfft(Data);     
					}
					drawflg   = true;
				}
				else if(MousePositionX< TOTAL_SCREEN_W and MousePositionY>HALF_HIGH_LINE)
				{				
					TypeOfGraphFlag--;
					if(!TypeOfGraphFlag)
					{
						TypeOfGraphFlag = 4;
					}
					drawflg   = true;
				}
				else if(MousePositionX< TOTAL_SCREEN_W and MousePositionY<HALF_HIGH_LINE)
				{
					DrawModeRegion++;
					if(DrawModeRegion>PHASE)
					{
						DrawModeRegion=1;
					}
					drawflg = true;
				}

				clearmouseclick(WM_RBUTTONDOWN);				  
				XposStart = XposEnd;
				FlagScreenClick = false;
				DrawModeFlag = true;
				ClearTheData = true;
			}			 
			char ch = 0;

			clearmouseclick(WM_LBUTTONDOWN);  
			setbkcolor(0xaaaaaa); 
			setcolor(0xaa);  
			outtextxy(TOTAL_SCREEN_W+5,5,"[+] Coordinates:");
			setcolor(0xff0000);
			sprintf((char*)CoordinatesStr.c_str(),"X:%05d",mousex());
			outtextxy(TOTAL_SCREEN_W+120,5,(char*)CoordinatesStr.c_str());
			sprintf((char*)CoordinatesStr.c_str(),"Y:%05d",(mousey()));
			outtextxy(TOTAL_SCREEN_W+190,5,(char*)CoordinatesStr.c_str());
			//settextstyle(11,0,1);
			sprintf((char*)CoordinatesStr.c_str(),"StepSize:%03d",rotationStep);				
			outtextxy(TOTAL_SCREEN_W+200,145,(char*)CoordinatesStr.c_str());
			//settextstyle(4,0,1);
					
		  }
	closegraph(); 
	return 0;
}


void Normalize(Complex * x)
	{
		double 	maxReal = 0,
				maxImag = 0;					
								
		for(int c = 0 ; c < DataLength; c++)
		{				   
			if (abs(x[c].real()) > maxReal)
			{
				maxReal = abs(x[c].real());
			}
			if (abs(x[c].imag()) > maxImag)
			{
				maxImag = abs(x[c].imag());
			}
		}   
				
		for(int c = 0 ; c < DataLength; c++) 
		{				  
			 x[c].real() = x[c].real() / maxReal;
			 x[c].imag() = x[c].imag() / maxImag;
		}
					 
	}





void shiftfft(Complex * x)
	{
		Complex tmp[DataLength];
		int half = DataLength/2;
		for(int c = 0; c<half;c++)		     
		{
			tmp[c]=x[c];		 
		}
		for(int c = 0 , s = half; c < half; c++,s++)
		{
			x[c]  = x[s];
		}
		for(int c = 0 , s = half; c < half; c++,s++)
		{
			x[s]  = tmp[c];
		}
	}



void Filter(Complex *x,int FStart,int FEnd)
	{
		for(int c = 0 ; c < DataLength;c++)
		{
			if(!(c>=FStart and c<=FEnd))
			{
				x[c] = 0;		   
			}
		}
	}















 
void fft(Complex * x)
	{
		unsigned int N = DataLength, k = N, n;
		double thetaT = 3.14159265358979323846264338328L / N;
		Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
		while (k > 1)
		{
			n = k;
			k >>= 1;
			phiT = phiT * phiT;
			T = 1.0L;
			for (unsigned int l = 0; l < k; l++)
			{
				for (unsigned int a = l; a < N; a += n)
				{
					unsigned int b = a + k;
					Complex t = x[a] - x[b];
					x[a] += x[b];
					x[b] = t * T;
				}
				T *= phiT;
			}
		}
	// Decimate
		unsigned int m = (unsigned int)log2(N);
		for (unsigned int a = 0; a < N; a++)
		{
			unsigned int b = a;
			// Reverse bits
			b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
			b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
			b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
			b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
			b = ((b >> 16) | (b << 16)) >> (32 - m);
			if (b > a)
			{
				Complex t = x[a];
				x[a] = x[b];
				x[b] = t;
			}
		}
	}




void RoL(Complex *x)
	{
		int tmp = rotationStep ;
		Complex Signal[tmp];
		
		for(int i = 0 ; i < tmp ; i++)
		{
			Signal[i]=x[i];
		}

		for(int i = 0 ; i < DataLength-tmp;i++)
		{
			x[i] = x[i+tmp];
		}
		for(int i = DataLength - tmp,c = 0 ; i < DataLength;i++,c++)
		{
			x[i] = Signal[c];				
		}
    }
    
	
	
	
	
	
void RoR(Complex *x)
	{
		int tmp = rotationStep ;
		Complex Signal[tmp];
		for(int i = DataLength - tmp,c = 0 ; i < DataLength;i++,c++)
		{
			Signal[c] = x[i]; 
		}
		for(int i = DataLength - 1 ; i >=tmp ;i--)
		{
			x[i] = x[i-tmp];
		}
		for(int i = 0 ; i < tmp ; i++)
		{
			x[i]=Signal[i];				   
		}
    }
 
 
 
 
 
 
 
void ifft(Complex * x)
	{
		// conjugate the complex numbers
		for (int c=0 ; c < DataLength;c++)
		{
			x[c] = conj(x[c]);
		}
	 
		// forward fft
		fft( x );
	 
		// conjugate the complex numbers again
		for (int c=0 ; c < DataLength;c++)
		{
			x[c] = conj(x[c]);
		}
		
		Normalize(x);		
	} 
 
 
 
 
 



