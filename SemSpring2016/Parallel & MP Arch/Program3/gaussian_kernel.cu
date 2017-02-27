__global__ void
Gaussian_CUDA(int* A, int wA, int row, int* temp)
{
        int bx, tx, by, ty;
        int i;
        float tmp;

        // Block index
        bx = blockIdx.x;
        by = blockIdx.y;

        // Thread index
        tx = threadIdx.x;
        ty = threadIdx.y;

        int index = wA * BLOCK_SIZE * by + BLOCK_SIZE * bx + wA * ty + tx;

        int index_col = BLOCK_SIZE * bx + tx;
        int index_row = BLOCK_SIZE * by + ty;

        if (by == 0 && ty == 0)
        temp[index] = A[index + row * wA];
        __syncthreads();

        for(i = row + 1; i < wA; i++)
       {
        if ( by == 0 && ty == 0)
        tmp = A[i * wA + row]/temp[row];
        __syncthreads();


        if ( by == 0 && ty == 0)
        A[i * wA + index] = A[i * wA + index] - temp[index] * tmp;

        __syncthreads();
       }
}
