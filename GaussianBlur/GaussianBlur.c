#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.14


//定义结构体
typedef struct{
    int r, g, b;
}PIXEL;

typedef struct{
    int width;
    int height;
    int colorset;
    PIXEL *data; 
}PPM;

//定义变量
const char *read_path ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\GaussianBlur\\apple.ppm";//写文件地址
const char *write_path ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\GaussianBlur\\apple_gaussianblur.ppm";//同一文件夹下，写输出文件的文件名

PPM inPPM;
PPM outPPM;

//检查错误
int ERROR_STATE =0;

enum{
    ERR_FILE_NOT_FOUND = 1,
	ERR_WRONG_FORMAT_HEADER,
	ERR_ILLEGAL_SIZE,
	ERR_FILE_BROKEN,
	ERR_FAILED_TO_WRITE,
};


int checkerror()
{
    return ERROR_STATE;
}

void throwerror(int error)
{
    ERROR_STATE = error;

}

//读取数据
void read()
{
    if(freopen(read_path,"r",stdin)==NULL)
    {
        throwerror(ERR_FILE_NOT_FOUND);
        return;
    }

   char *FARMAT_HEADER = malloc(sizeof(char*));
   scanf("%s",FARMAT_HEADER);
   if(strcmp(FARMAT_HEADER,"P3") != 0)
   {
        throwerror(ERR_WRONG_FORMAT_HEADER);
        return;
   }

   int width,height,colorset;
   scanf("%d%d%d",&width,&height,&colorset);
   if(width<=0 || height <=0 )
   {
        throwerror(ERR_ILLEGAL_SIZE);
        return;
   }

    inPPM.width = width;
    inPPM.height = height;
	inPPM.colorset = colorset;
	inPPM.data = malloc(sizeof(PIXEL) * width *height);
	int r, g, b;
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            if (scanf("%d%d%d", &r, &g, &b)==3) {//scanf 返回成功读取的项目数量（0-3）
				inPPM.data[x + y * width].r = r;//二维数组的一维化存储方式，代表第x列第y行的像素
				inPPM.data[x + y * width].g = g;
				inPPM.data[x + y * width].b = b;
			}
            else{
                throwerror(ERR_FILE_BROKEN);
                return;
            }
        }
    }

    fclose(stdin);
}

//输出数据
void write()
{
    if (checkerror()) {
		return;
	}

	FILE* file = fopen(write_path, "wb");//"wb"二进制
	if (file == NULL) {
		throwerror(ERR_FAILED_TO_WRITE);
		return;
	}

    int flag=0;
    //头部份
    flag |= 0>= fprintf(file, "P3\n"); //flag = flag | (0 >= fprintf(...))
	flag |= 0>= fprintf(file, "%d %d\n", outPPM.width, outPPM.height);
	flag |= 0>= fprintf(file, "%d\n", outPPM.colorset);
    //图像数据部分
    int N=outPPM.width * outPPM.height;
    for(int i=0; i<N; i++)
    {
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].r);
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].g);
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].b);
    }
    if(flag==0){
        throwerror(ERR_FAILED_TO_WRITE);
        return;
    }

    fflush(file);//强制把缓冲区数据写入磁盘
	fclose(file);//关闭文件
    
}


PIXEL BLACK = {0, 0, 0};

//获得（i,j）的rgb
PIXEL getPixel(PPM *source, int x, int y) {
    if (x < 0 || y < 0 || x >= source->width || y >= source->height) {
        return BLACK;
    }
    return source->data[x + y * source->width];
}

//算权重
double gaussian2D(int i, int j)
{
    double sigma = 1.5;
    double sigma2 = sigma * sigma;
    double weight;
    weight = 1.0/(2*PI*sigma2)*exp(-(i*i + j*j) / (2*sigma2));
    return weight;
}

//高斯模糊
PIXEL gaussianblur(PPM *source, int x, int y, int r)
{
    PIXEL blur_pixel;
    blur_pixel.r = 0;
    blur_pixel.g = 0;
    blur_pixel.b = 0;
    PIXEL temp;
    double total_weight = 0.0;

    for(int i=-r; i<=+r; i++){
        for(int j=-r; j<=+r; j++){
            temp = getPixel(source, x+i, y+j);//得到当前（x+i, y+j）点像素的rgb
            double weight = gaussian2D(i, j);
            blur_pixel.r += temp.r * weight;
            blur_pixel.g += temp.g * weight;
            blur_pixel.b += temp.b * weight;

            total_weight += weight;
        }
    }

    
    blur_pixel.r = (int)(blur_pixel.r / total_weight);
    blur_pixel.g = (int)(blur_pixel.g / total_weight);
    blur_pixel.b = (int)(blur_pixel.b / total_weight);
    

    return blur_pixel;
}

void handle(int x1, int y1, int x2, int y2)
{
    int width = inPPM.width, height = inPPM.height;
    outPPM.width = inPPM.width;
    outPPM.height = inPPM.height;
    outPPM.colorset = inPPM.colorset;
    outPPM.data = malloc(sizeof(PIXEL) * outPPM.width * outPPM.height);

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(x>=x1 && y>=y1 && x<=x2 && y<=y2){
                outPPM.data[x + y*width] = gaussianblur(&inPPM, x, y, 5);
            }
            else{
                outPPM.data[x + y*width] = inPPM.data[x + y*width];
            }
            
        }
    }

}



int main()
{
    read();
    handle(198,213,423,696);
    write();
    return ERROR_STATE;
}
