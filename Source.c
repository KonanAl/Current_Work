#pragma warning(disable : 4996)

// Konan : 24.06.2022, 10:00
// Original algorithm for 2D images
// from "Bovik, A.: Handbook of Image and Video Processing. Academic Press, London (2000), pages 60-62

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


typedef struct{
	int ** data;
	int height;
	int width;
}IMAGE2D;
//----------------------------------------------------------
void loadImage_ascii(IMAGE2D * image, const char * filename) {
    int nx,ny,intensity;
    int i,j;

    char line1[5];
    char line2[80];

    FILE * f = fopen(filename,"r"); // read only
    if (f==NULL){
        printf("File not found");
    }

    fgets(line1,10,f);

    do{
          fgets(line2,80,f);
    }while(line2[0]=='#');

    sscanf(line2,"%d %d",&ny,&nx);

    fgets(line2,10,f);

    image->width = ny;
    image->height = nx;

    image->data = (int **) malloc(nx * sizeof(int *));
    for (i=0; i<nx; i++) {
         image->data[i] = (int *) malloc(ny * sizeof(int));
         for (j=0;j<ny; j++) {
               fscanf(f,"%d",&intensity);
               image->data[i][j]=intensity;
          }
     }

     fclose(f);
}
//----------------------------------------------------------------
void saveImage_ascii(IMAGE2D * image, char * filename) {
    int i,j;
    FILE * f = fopen(filename,"w");

    int ny = image->width;
    int nx = image->height;

    fprintf(f,"P2\n");
    fprintf(f,"%d %d\n%d\n",image->width,image->height,255);

    for(i=0;i<nx;i++){
        for(j=0;j<ny;j++){
        	fprintf(f,"%d ", image->data[i][j]);
        }
    }
    fclose(f);
}
//----------------------------------------
void rescalingTo_0_255(int ** imageData, int h, int w){
	int i,j;
	double factor;
	int minD = INT_MAX;
	int maxD = INT_MIN;
	
	for(i=0; i<h; i++){
		for(j=0; j<w; j++){
			if(imageData[i][j]>maxD){
				maxD = imageData[i][j];
			}
			if(imageData[i][j]<minD){
				minD = imageData[i][j];
			}
		}
	}
	
	factor = 255./(maxD-minD);
	
	for(i=0; i<h; i++){
		for(j=0; j<w; j++){
			imageData[i][j] = (int)(factor*(imageData[i][j]-maxD)+255);
		}
	}
}


//----------------------------------------
void deleteImage2D(IMAGE2D * image) {
	int nx = image->height; 
	int i;
	for(i=0; i<nx; i++){
		free(image->data[i]);
	}
    free(image->data);
}
//---------------------------------------

int main(){
	
	IMAGE2D image, image2;
	int i,j,n,m;
		
	loadImage_ascii(&image2, "artificial.pgm");
	//loadImage_ascii(&image2, "memboa.pgm");
	//saveImage_ascii(&image2, "loaded.pgm");
	
	int Height = image2.height; 
	int Width = image2.width; 
	
	/*
	//Test rescalling
	for(i=0; i<Height; i++){
		for(j=0; j<Width; j++){
			image2.data[i][j] = image2.data[i][j]+100;
		}
	}
	saveImage_ascii(&image2, "addded.pgm");
	//exit(0);
	rescalingTo_0_255(image2.data, Height, Width);
	saveImage_ascii(&image2, "afterRescaling.pgm");
	*/
	
	//labels array
	int ** labelArray = (int **)malloc(Height * sizeof(int*));
	image.data = (int **)malloc(Height * sizeof(int *));
	for(i=0; i<Height; i++){
		labelArray[i] = (int *)malloc(Width * sizeof(int));
		image.data[i] = (int *)malloc(Width * sizeof(int));
	}
  	
  	//Initialization
  	image.height = Height; image.width = Width;
  	for (i=0; i<Height; i++) {
    	for (j=0; j<Width; j++) {
      		labelArray[i][j] = 0;
      		image.data[i][j] = image2.data[i][j];
    	}
  	}
  	//saveImage_ascii(&image, "loaded2.pgm");
  	
  	/*
  	//Thresholding
  	for(i=0; i<Height; i++){
		for(j=0; j<Width; j++){
			if(image.data[i][j]<102){
				image.data[i][j] = 255;
			}else{
				image.data[i][j] = 0;	
			}	
		}
	}
	saveImage_ascii(&image, "afterThresholding.pgm");
	*/
  	

  
  	//Labelling
  	int regionCounter = 1;
  	for (i=0; i<Height; i++) {
    	for (j=0; j<Width; j++) {
    	
    		if(i==0){
    			if(j==0){
    				if(image.data[i][j]==0){
    					labelArray[i][j] = regionCounter;
    					regionCounter++;
					}
				}else{
					if(image.data[i][j]==0){
						if(image.data[i][j-1]==255){
							labelArray[i][j] = regionCounter;
							regionCounter++; 
						}else{
							labelArray[i][j] = labelArray[i][j-1];
						}
					}		
				}
			}else{
				if(j==0){
					if(image.data[i][j]==0){
						if(image.data[i-1][j]==255){
							labelArray[i][j] = regionCounter;
							regionCounter++;	
						}else{
							labelArray[i][j] = labelArray[i-1][j];
						}
					}	
				}else{
					
					if(image.data[i][j]==0){
						if(image.data[i-1][j]== 255 && image.data[i][j-1]==255){
							labelArray[i][j] = regionCounter;
          					regionCounter++;	
						}
						if(image.data[i-1][j]==0 && image.data[i][j-1]==255){
							labelArray[i][j] = labelArray[i-1][j];	
						}
						if(image.data[i-1][j]==255 && image.data[i][j-1]==0){
							labelArray[i][j] = labelArray[i][j-1];	
						}
						if(image.data[i-1][j]==0 && image.data[i][j-1]==0){
							
								if(labelArray[i][j-1]==labelArray[i-1][j]){
									labelArray[i][j] = labelArray[i-1][j];
								}
								
								/*
								else{
			
								int max = labelArray[i-1][j];
								int min = labelArray[i][j-1];
								if(max  < labelArray[i][j-1]){
									max = labelArray[i][j-1];
									min = labelArray[i-1][j];
								}
								
								for(n=0; n<Height; n++){
									for(m=0; m<Width; m++){
										if(labelArray[n][m]==max){
											labelArray[n][m] = min; 
										}
									}
								}
								
						    	}
						    	*/
						    
							
							/*
							if(labelArray[i][j-1]>labelArray[i-1][j]){
								labelArray[i][j]= labelArray[i-1][j];
								for(n=0; n<Height; n++){
									for(m=0; m<Width; m++){
										if(labelArray[n][m]==labelArray[i][j-1]){
											labelArray[n][m] = labelArray[i-1][j]; 
										}
									}
								}	
							}else{
								if(labelArray[i][j-1]==labelArray[i-1][j]){
									labelArray[i][j] = labelArray[i-1][j];
								}else{
									labelArray[i][j]= labelArray[i][j-1];
									for(n=0; n<Height; n++){
										for(m=0; m<Width; m++){
											if(labelArray[n][m]==labelArray[i-1][j]){
												labelArray[n][m] = labelArray[i][j-1]; 
											}else{
												continue;
											}
										}
									}
								}
							}
							*/
							
						}     		
					}		
				}
			}
		}
	}
	
	printf("RegionCounter = %d \n ", regionCounter); 
	
	for(i=0; i<Height; i++){
		for(j=0; j<Width; j++){
			image2.data[i][j]=labelArray[i][j];
		}
	}
	
	/*
	// Non zero labels
	int nonZeroLabels = 0;
	for(i=0; i<Height; i++){
		for(j=0; j<Width; j++){
			if(labelArray[i][j]!=0){
				nonZeroLabels++;
			}
		}
	}
	printf("Number of non zero labels = %d", nonZeroLabels);
	*/
	
	rescalingTo_0_255(image2.data, Height, Width);
	saveImage_ascii(&image2, "segmentedImage.pgm");
	
	
	//Saving of label array as csv
	FILE * file1 = fopen("labelArray.csv","w");
	for(i=0; i<Height; i++){
		for(j=0; j<Width; j++){
			fprintf(file1, "%d", labelArray[i][j]);
			fprintf(file1, "  ");
		}
		fprintf(file1, "\n");
	}
	fclose(file1);
	
	// Region counting
	int * Counting = (int *)malloc(regionCounter * sizeof(int));
	for (i=0; i<regionCounter; i++) {
  		Counting[i] = 0;
	}
  	for (i=0; i<Height; i++) {
    	for (j=0; j<Width; j++) {
      		if (image.data[i][j] == 0 ) {
        		Counting[labelArray[i][j]] = Counting[labelArray[i][j]] + 1;
      		}else {
        		continue;
      		}
    	}
  	}
  	
  	/*
  	int countRealRegion = 0;
  	for(i=0; i<regionCounter; i++){
  			if(Counting[i]!=0){
  				countRealRegion++;
			}
	}	
	printf("number of real Regions = %d", countRealRegion);
	*/
	
  	
  	FILE * file2 = fopen("Counting.csv","w");
  	fprintf(file2, "Regions   Total \n");
	for(i=0; i<regionCounter; i++){
			fprintf(file2, "%d          %d", i, Counting[i]);
			fprintf(file2, "\n");
	}
	fclose(file2);

	//Minor region removal
	int S = 500; // minimal size to be removed
	
  	for (i=0; i<Height; i++) {
    	for (j=0; j<Width; j++) {
      		if (Counting[labelArray[i][j]] < S) {
        		image.data[i][j] = 255;
      		}
    	}
  	}
  	
  	saveImage_ascii(&image, "ImageAfterRemoving500.pgm");
  	
  	for(i=0; i<Height; i++){
  		free(labelArray[i]); 
	}
	free(labelArray);
	free(Counting); 
	
	deleteImage2D(&image); deleteImage2D(&image2);
  
	return 0;
}
