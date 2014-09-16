void ForwardSub(int N,  float a[restrict const static N][N], float b[restrict const static N], float m[restrict const static N][N]){
#pragma scop
  for (int i = 0;  i< N-1; i++ )
  {
    for( int j = 0; j < N-1-i ; j++ ){
          m[i + j + 1][i] = a[i+j+1][i] / a[i][i];
    }

    for( int j = 0; j < N; j++ )
    {
      for( int k = 0; k < N; k++ )
      {
          if (j < N-1-i && k < N-i)
            a[j+1+i][i+k] -= m[j+1+i][i] * a[i][i+k];

          if(k == 0)
            b[j+1+i] -= m[j+1+i][i+k] * b[i];

      }
    }
  }
#pragma endscop
}
