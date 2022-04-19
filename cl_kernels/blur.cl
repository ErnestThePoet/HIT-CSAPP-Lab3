#define BYTES_PER_PX 3

kernel void BlurNaive(
    const global uchar* source_pixel_data,
    global uchar* dest_pixel_data,
    const int width,
    const int height){
    size_t col_id=get_global_id(0);
    size_t row_id=get_global_id(1);

    size_t bytes_per_row=((width*BYTES_PER_PX-1)/4+1)*4;

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

constant sampler_t image_sampler=
    CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP_TO_EDGE|CLK_FILTER_NEAREST;

// here we guarantee that each byte in each row is pixel data, not bmp padding
// known issues: edge pixel may have wrong colors because their B channels aren't handeled.
kernel void BlurImage(
    read_only image2d_t source_image,
    write_only image2d_t dest_image,
    const int width_reinterpret,
    const int height){
    size_t col_id=get_global_id(0);
    size_t row_id=get_global_id(1);

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

#define WPI4 4

kernel void BlurImageWPI4(
    read_only image2d_t source_image,
    write_only image2d_t dest_image,
    const int width_reinterpret,
    const int height){
    // first col id of current block
    size_t col_id=1+get_global_id(0)*WPI4;
    size_t row_id=get_global_id(1);

    if(row_id>0&&row_id<height-1&&col_id+WPI4<width_reinterpret){ 
        // col_id+WPI4-1<width_reinterpret-1, make sure right-most element to process is within boundary

        uint16 upper_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id-1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id-1))
        );
        uint16 left_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id-1,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id))
        );
        uint16 center_pixels=(uint16)(
            left_pixels.s4567,
            left_pixels.hi,
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id))
        );
        uint16 right_pixels=(uint16)(
            center_pixels.s4567,
            center_pixels.hi,
            read_imageui(source_image,image_sampler,(int2)(col_id+4,row_id))
        );
        uint16 lower_pixels=(uint16)(
            read_imageui(source_image,image_sampler,(int2)(col_id,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+1,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+2,row_id+1)),
            read_imageui(source_image,image_sampler,(int2)(col_id+3,row_id+1))
        );

        uint16 pixel_data_sum=
            (upper_pixels
            +(uint16)(left_pixels.s1,left_pixels.s23,left_pixels.lo.hi,left_pixels.hi,center_pixels.s0)
            +(uint16)(center_pixels.s3,right_pixels.lo,right_pixels.hi.lo,right_pixels.scd,right_pixels.se)
            +lower_pixels)>>2;

        write_imageui(dest_image,(int2)(col_id,row_id),pixel_data_sum.lo.lo);
        write_imageui(dest_image,(int2)(col_id+1,row_id),pixel_data_sum.lo.hi);
        write_imageui(dest_image,(int2)(col_id+2,row_id),pixel_data_sum.hi.lo);
        write_imageui(dest_image,(int2)(col_id+3,row_id),pixel_data_sum.hi.hi);
    }
}