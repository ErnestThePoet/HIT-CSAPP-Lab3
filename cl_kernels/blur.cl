#define BYTES_PER_PX 3
#define WPI4 4
#define WPI8 8
#define WPI16 16



kernel void BlurNaive(
    const global uchar* source_pixel_data,
    global uchar* dest_pixel_data,
    const int width,
    const int height){
    
    const size_t col_id=get_global_id(0);
    const size_t row_id=get_global_id(1);

    const size_t bytes_per_row=((width*BYTES_PER_PX-1)/4+1)*4;

    if(row_id>0&&row_id<height-1&&col_id>0&&col_id<width-1){

        size_t total_offset=row_id*bytes_per_row+col_id*BYTES_PER_PX;
        size_t upper_total_offset=total_offset-bytes_per_row;
        size_t lower_total_offset=total_offset+bytes_per_row;
        

        dest_pixel_data[total_offset]=
            ((uint)source_pixel_data[upper_total_offset]
            +source_pixel_data[total_offset-BYTES_PER_PX]
            +source_pixel_data[total_offset+BYTES_PER_PX]
            +source_pixel_data[lower_total_offset])>>2;

        dest_pixel_data[total_offset+1]=
            ((uint)source_pixel_data[upper_total_offset+1]
                +source_pixel_data[total_offset-BYTES_PER_PX+1]
                +source_pixel_data[total_offset+BYTES_PER_PX+1]
                +source_pixel_data[lower_total_offset+1])>>2;

        dest_pixel_data[total_offset+2]=
            ((uint)source_pixel_data[upper_total_offset+2]
            +source_pixel_data[total_offset-BYTES_PER_PX+2]
            +source_pixel_data[total_offset+BYTES_PER_PX+2]
            +source_pixel_data[lower_total_offset+2])>>2;
    }
}



/////////////////////////////////////////////////////////////////////////
// This version is not used as it is even slower than Naive.
// But I preserve the code because it has also seen my efforts.
/////////////////////////////////////////////////////////////////////////
// 16 bytes per work-item
// kernel void BlurWPI16(
//     const global uchar* source_pixel_data,
//     global uchar* dest_pixel_data,
//     const int width,
//     const int height){
    
//     // first byte (not pixel) col id of current block
//     const size_t col_id=BYTES_PER_PX+get_global_id(0)*WPI16;
//     // this is pixel or byte (actually same!) row id
//     const size_t row_id=get_global_id(1);

//     const size_t bytes_per_row=((width*BYTES_PER_PX-1)/4+1)*4;
//     const size_t data_byte_ubound=width*BYTES_PER_PX-BYTES_PER_PX;

//     if(row_id>0&&row_id<height-1&&col_id+WPI16-1<data_byte_ubound){

//         size_t total_offset=row_id*bytes_per_row+col_id;
//         size_t upper_total_offset=total_offset-bytes_per_row;
//         size_t lower_total_offset=total_offset+bytes_per_row;

//         /*
//                 [************  ****] <-UPPER PIXELS (16)
//             [****************][********] <-CENTER PIXELS (16), CENTER PIXELS1 (8)
//                 [************  ****] <-LOWER PIXELS (16)
//         */

//         // already upper pixels
//         uint16 upper_pixels=convert_uint16(vload16(0,source_pixel_data+ upper_total_offset));
//         uint16 center_pixels=convert_uint16(vload16(0,source_pixel_data+ total_offset-4));
//         uint8 center_pixels_1=convert_uint8(vload8(0,source_pixel_data+ total_offset+WPI16-4));
//         uint16 lower_pixels=convert_uint16(vload16(0,source_pixel_data+ lower_total_offset));


//         uchar16 avg_pixels=convert_uchar16((
//             upper_pixels
//             +(uint16)(center_pixels.yzw,center_pixels.lo.hi,center_pixels.hi,center_pixels_1.x)
//             +(uint16)(center_pixels.lo.hi.w,center_pixels.hi,center_pixels_1.lo,center_pixels_1.hi.xyz)
//             +lower_pixels)>>2);

//         vstore16(avg_pixels,0,dest_pixel_data+ total_offset);
//     }
// }



constant sampler_t image_sampler=
    CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

// here we guarantee that each byte in each row is pixel data, not bmp padding
// known issues: edge pixel may have wrong colors because their B channels aren't handeled.
kernel void BlurImage(
    read_only image2d_t source_image,
    write_only image2d_t dest_image,
    const int width_reinterpret,
    const int height){
    
    const size_t col_id=get_global_id(0);
    const size_t row_id=get_global_id(1);

    if(row_id>0&&row_id<height-1&&col_id>0&&col_id<width_reinterpret-1){

        // Note that col_id is in x axis of image; row_id is in y axis of image.
        //  x 0 1...
        // y
        // 0
        // 1
        // ...
        
        uint4 upper_pixel=read_imageui(source_image,image_sampler,(int2)(col_id,row_id-1));
        uint4 left_pixel=read_imageui(source_image,image_sampler,(int2)(col_id-1,row_id));
        uint4 center_pixel=read_imageui(source_image,image_sampler,(int2)(col_id,row_id));
        uint4 right_pixel=read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id));
        uint4 lower_pixel=read_imageui(source_image,image_sampler,(int2)(col_id,row_id+1));

        uint4 pixel_data_sum=
            upper_pixel
            +(uint4)(left_pixel.yzw,center_pixel.x)
            +(uint4)(center_pixel.w,right_pixel.xyz)
            +lower_pixel;

        write_imageui(dest_image,(int2)(col_id,row_id),pixel_data_sum>>2);
    }
}



kernel void BlurImageWPI4(
    read_only image2d_t source_image,
    write_only image2d_t dest_image,
    const int width_reinterpret,
    const int height){
    // first pixel col id of current block
    const size_t col_id=1+get_global_id(0)*WPI4;
    const size_t row_id=get_global_id(1);

    if(row_id>0&&row_id<height-1&&col_id+WPI4<width_reinterpret){ 
        // col_id+WPI4-1<width_reinterpret-1, make sure right-most element to process is within boundary

        /*
              [UPPER  PIXELS]
           [L][CENTER PIXELS][R]
              [LOWER  PIXELS]
        */

        uint16 upper_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id-1))
        );

        uint4 left_pixel=read_imageui(source_image,image_sampler,(int2)(col_id-1,row_id));

        uint16 center_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id))
        );

        uint4 right_pixel=read_imageui(source_image,image_sampler,(int2)(col_id+4,row_id));

        uint16 lower_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id+1))
        );

        uint16 pixel_data_sum=
            (upper_pixels
            +(uint16)(left_pixel.s1,left_pixel.s23,center_pixels.lo,center_pixels.hi.lo,center_pixels.hi.hi.x)
            +(uint16)(center_pixels.s3,center_pixels.lo.hi,center_pixels.hi,right_pixel.xyz)
            +lower_pixels)>>2;

        // if(row_id==1){
        //             printf("ROW=%d COL=%d R=%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        // (int)row_id,(int)col_id,right_pixels.s0,right_pixels.s1,right_pixels.s2,right_pixels.s3,right_pixels.s4,
        // right_pixels.s5,right_pixels.s6,right_pixels.s7,right_pixels.s8,right_pixels.s9,right_pixels.sa,
        // right_pixels.sb,right_pixels.sc,right_pixels.sd,right_pixels.se,right_pixels.sf);
        // }

        write_imageui(dest_image,(int2)(col_id,row_id),pixel_data_sum.lo.lo);
        write_imageui(dest_image,(int2)(col_id+1,row_id),pixel_data_sum.lo.hi);
        write_imageui(dest_image,(int2)(col_id+2,row_id),pixel_data_sum.hi.lo);
        write_imageui(dest_image,(int2)(col_id+3,row_id),pixel_data_sum.hi.hi);
    }
}



kernel void BlurImageWPI8(
    read_only image2d_t source_image,
    write_only image2d_t dest_image,
    const int width_reinterpret,
    const int height){
    // first pixel col id of current block
    const size_t col_id=1+get_global_id(0)*WPI8;
    const size_t row_id=get_global_id(1);

    if(row_id>0&&row_id<height-1&&col_id+WPI8<width_reinterpret){ 
        // col_id+WPI8-1<width_reinterpret-1, make sure right-most element to process is within boundary

        /*
              [UPPER  PIXELS][UPPER  PIXELS1]
           [L][CENTER PIXELS][CENTER PIXELS1][R]
              [LOWER  PIXELS][LOWER  PIXELS1]
        */

        uint16 upper_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id-1))
        );

        uint16 upper_pixels_1=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id+4,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+5,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+6,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+7,row_id-1))
        );
        
        uint4 left_pixel=read_imageui(source_image,image_sampler,(int2)(col_id-1,row_id));

        uint16 center_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id))
        );

        uint16 center_pixels_1=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id+4,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+5,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+6,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+7,row_id))
        );

        uint4 right_pixel=read_imageui(source_image,image_sampler,(int2)(col_id+8,row_id));

        uint16 lower_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id+1))
        );
        uint16 lower_pixels_1=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id+4,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+5,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+6,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+7,row_id+1))
        );

        uint16 pixel_data_sum=
            (upper_pixels
            +(uint16)(left_pixel.s1,left_pixel.s23,center_pixels.lo,center_pixels.hi.lo,center_pixels.hi.hi.x)
            +(uint16)(center_pixels.s3,center_pixels.lo.hi,center_pixels.hi,center_pixels_1.xyz)
            +lower_pixels)>>2;

        uint16 pixel_data_sum_1=
            (upper_pixels_1
            +(uint16)(center_pixels.sd,center_pixels.sef,center_pixels_1.lo,center_pixels_1.hi.lo,center_pixels_1.hi.hi.x)
            +(uint16)(center_pixels_1.s3,center_pixels_1.lo.hi,center_pixels_1.hi,right_pixel.xyz)
            +lower_pixels_1)>>2;

        write_imageui(dest_image,(int2)(col_id,row_id),pixel_data_sum.lo.lo);
        write_imageui(dest_image,(int2)(col_id+1,row_id),pixel_data_sum.lo.hi);
        write_imageui(dest_image,(int2)(col_id+2,row_id),pixel_data_sum.hi.lo);
        write_imageui(dest_image,(int2)(col_id+3,row_id),pixel_data_sum.hi.hi);
        write_imageui(dest_image,(int2)(col_id+4,row_id),pixel_data_sum_1.lo.lo);
        write_imageui(dest_image,(int2)(col_id+5,row_id),pixel_data_sum_1.lo.hi);
        write_imageui(dest_image,(int2)(col_id+6,row_id),pixel_data_sum_1.hi.lo);
        write_imageui(dest_image,(int2)(col_id+7,row_id),pixel_data_sum_1.hi.hi);
    }
}