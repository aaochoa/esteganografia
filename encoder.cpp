#include <highgui.h>
#include <bits/stdc++.h>
#include <cv.h>
#include <omp.h>

using namespace cv;
using namespace std;

string archivo,foto;
Mat image;  // Read the file, BGR format
// para hacer: leer cualquier formato (especificado) y decodificar a ese formato

//==============================================================================

int fsize()
{
    FILE * f = fopen(archivo.c_str(), "r");
    fseek(f, 0, SEEK_END);
    int len = (int)ftell(f);
    fclose(f);
    return len;
}

//==============================================================================
string DecToBin2(int number)
{
    string result = "";

    do
    {
        if ( (number & 1) == 0 )
            result += "0";
        else
            result += "1";

        number >>= 1;
    } while ( number );

    reverse(result.begin(), result.end());
    return result;
}


//==============================================================================
// insertamos a la imagen el tamaño en bits de la cantidad de caracteres a leer. ejemplo : 3 caracteres -> necesita 000011 (2)
// reservamos: 6 bits como tamaño máximo de representación.
int step_one(int height,int width,uchar *image_example_data,uchar *image_data,int nc)
{
  int j=0;
  string binary_nc = DecToBin2(nc);
  string binary_ar = bitset<6>(binary_nc.length()).to_string();

#pragma omp parallel
{
  #pragma omp for
  for(j;j<2;j++)
  {

    // obtenemos información.
    short int B=(int)image_data[(j)*3+0];
    short int G=(int)image_data[(j)*3+1];
    short int R=(int)image_data[(j)*3+2];
    //cout<<"Pixel: "<<j<<"x"<<i<<" B: "<<B<<" G: "<<G<<" R: "<<R<<endl;
    // Pasamos a binario
    string binary_blue = bitset<8>(B).to_string();
    string binary_green = bitset<8>(G).to_string();
    string binary_red = bitset<8>(R).to_string();
    // Modificamos binario
    binary_blue[7]=binary_ar[j*3];
    binary_green[7]=binary_ar[j*3+1];
    binary_red[7]=binary_ar[j*3+2];
    // creamos nueva imágen.
    image_example_data[(j)*3]=(int)bitset<8>(binary_blue).to_ulong();
    image_example_data[(j)*3+1]=(int)bitset<8>(binary_green).to_ulong();
    image_example_data[(j)*3+2]=(int)bitset<8>(binary_red).to_ulong();
    //cout<<"Pixel: "<<j<<"x"<<i<<" Binary_b: "<<bitset<8>(binary_blue).to_ulong()<<" binary_G: "<<bitset<8>(binary_green).to_ulong()<<" binary_R: "<<bitset<8>(binary_red).to_ulong()<<endl;

  }
}
  return binary_nc.length();

}

//==============================================================================
void step_two(int height,int width,uchar *image_example_data,uchar *image_data,int lnc,int nc)
{

  string binary = DecToBin2(nc);
  int i=0,j=2;
  for(int k=0;k<ceil((float)lnc/3.0);k++)
  {

    if(j%width==0 && j!=0)
    {
         i++;
         j=0;
     }

     // obtenemos información.
     short int B=(int)image_data[(i*width+j)*3+0];
     short int G=(int)image_data[(i*width+j)*3+1];
     short int R=(int)image_data[(i*width+j)*3+2];
     //cout<<"Pixel: "<<j<<"x"<<i<<" B: "<<B<<" G: "<<G<<" R: "<<R<<endl;
     // Pasamos a binario
     string binary_blue = bitset<8>(B).to_string();
     string binary_green = bitset<8>(G).to_string();
     string binary_red = bitset<8>(R).to_string();
     //cout<<"Pixel: "<<j<<"x"<<i<<" Binary_b: "<<bitset<8>(binary_blue).to_ulong()<<" binary_G: "<<bitset<8>(binary_green).to_ulong()<<" binary_R: "<<bitset<8>(binary_red).to_ulong()<<endl;
     //cout<<"binary"<<binary<<endl;
     // creamos nueva imágen.
     if((i*width+k)*3 < lnc)
     {
       binary_blue[7]=binary[k*3];
       image_example_data[(i*width+j)*3]=(int)bitset<8>(binary_blue).to_ulong();
     }else
     {
       image_example_data[(i*width+j)*3]=B; // creo que nunca se cumplirá, pero no es problema
     }

     if((i*width+k)*3+1 < lnc)
     {
       binary_green[7]=binary[k*3+1];
       image_example_data[(i*width+j)*3+1]=(int)bitset<8>(binary_green).to_ulong();
     }else
     {
       image_example_data[(i*width+j)*3+1]=G;
     }

     if((i*width+k)*3+2 < lnc)
     {
       binary_red[7]=binary[k*3+2];
       image_example_data[(i*width+j)*3+2]=(int)bitset<8>(binary_red).to_ulong();
     }else
     {
       image_example_data[(i*width+j)*3+2]=R;
     }

     j++;
     //cout<<"Pixel: "<<j<<"x"<<i<<" Binary_b: "<<bitset<8>(binary_blue).to_ulong()<<" binary_G: "<<bitset<8>(binary_green).to_ulong()<<" binary_R: "<<bitset<8>(binary_red).to_ulong()<<endl;
  }

  cout<<"Ancho de la imágen: "<<width<<endl;
  cout<<"Alto de la imágen: "<<height<<endl;
  int number_of_character=nc;

  // :::::::::::::::::::::::: Coding

  ifstream file;
  file.open(archivo.c_str());
  //file.open("big.txt");
  char my_character;
  int number_of_lines = 0;

  while (!file.eof())
  {

    file.get(my_character);

      string binary_char = bitset<8>((int)my_character).to_string();
      //cout<<my_character<<endl;

      for(int k = 0 ;k < 3 ; k++)
      {

         if(j%width==0 && j!=0)
         {
              i+=1;
              j=0;
          }

          short int B=(int)image_data[(i*width+j)*3+0];
          short int G=(int)image_data[(i*width+j)*3+1];
          short int R=(int)image_data[(i*width+j)*3+2];

          string binary_blue = bitset<8>(B).to_string();
          string binary_green = bitset<8>(G).to_string();
          string binary_red = bitset<8>(R).to_string();

          binary_blue[7]=binary_char[k*3];
          binary_green[7]=binary_char[k*3+1];

          //cout<<"blue: "<<binary_blue.c_str()<<endl;
          //cout<<"green: "<<binary_green.c_str()<<endl;

          image_example_data[(i*width+j)*3]=(int)bitset<8>(binary_blue).to_ulong();
          image_example_data[(i*width+j)*3+1]=(int)bitset<8>(binary_green).to_ulong();

          if(k<2)
          {
            binary_red[7]=binary_char[k*3+2];
            image_example_data[(i*width+j)*3+2]=(int)bitset<8>(binary_red).to_ulong();
            //cout<<"red: "<<bitset<8>(binary_red).to_ulong()<<endl;
          }else
          {
            image_example_data[(i*width+j)*3+2]=R;
          }//cout<<"red: "<<R<<endl;

          j++;

      if (my_character == '\n')
      {
        ++number_of_lines;
      }
    }// end for
      if(i == height){break;}
  } // end while

  // rest of data
  //cout<<"i "<<i<<" j "<<j<<endl;

  if(i<height || j < width)
  {
      while (i<height)
      { // y
        while(j < width)
        { // x
          Vec3f intensity = image.at<Vec3b>(i, j); //(y,x)
          int blue = intensity.val[0];
          int green = intensity.val[1];
          int red = intensity.val[2];
          image_example_data[(i*width+j)*3]=blue;
          image_example_data[(i*width+j)*3+1]=green;
          image_example_data[(i*width+j)*3+2]=red;
          //cout<<"Pixel "<<j<<"x"<<i<<" B: "<<blue<<" G: "<<green<<" R: "<<red<<endl;
          j++;
        }
        j=0;
        i++;
   }
 }

 if((( width * height ) / 8) < (double)number_of_character )
 {
   cout<<"No hay suficientes píxeles para representar su mensaje completo"<<endl;
   cout<<"Elimine: "<<(double)number_of_character - (( width * height ) / 8)<<" Caracteres"<<endl;
   double total_pixels=(( width * height ) *  (double)number_of_character) ;
   double div1 = (( width * height )/8) ;
   cout<<"Se necesita una imagen de "<<total_pixels/div1<<" total píxeles para representar su mensaje completamente"<<endl;

   //return 0;
 }else
 {
   cout<<"su mensaje fue representado correctamente y en su totalidad"<<endl;
 }


}

//==============================================================================
int main(int argc, char** argv)
{

  clock_t start, finish; //Clock variables
  double elapsedSequential;
  archivo="./alice.txt";
  foto="./example.jpg";
  //archivo="./inputs/alice.txt";
  //foto="inputs/img6.jpg";


  cout<<"archivo a codificar: "<<archivo<<endl;
  cout<<"imagen  de entrada: "<<foto<<endl;

  image = imread(foto,CV_LOAD_IMAGE_COLOR);

  Mat image_example;
  uchar *image_data;
  Size s = image.size();
  int width = s.width;
  int height= s.height;
  uchar *image_example_data = (unsigned char *)malloc(sizeof(unsigned char)*(width*height)*3); // 3 channels
  image_data = image.data;

  start = clock();

  int nc = fsize();
  int lnc = step_one(height,width,image_example_data,image_data,nc);
  step_two(height,width,image_example_data,image_data,lnc,nc);

  finish = clock();

  elapsedSequential = (((double) (finish - start)) / CLOCKS_PER_SEC );
  cout<< "The Secuential process took: " << elapsedSequential << " seconds to execute "<< endl;

  image_example.create(height,width,CV_8UC3);  // 8 bits 3 Channels
  image_example.data = image_example_data;
  imwrite("./1053823121.png",image_example);
  //imwrite("./outputs/1053823121.png",image_example);

  return 0;
}
