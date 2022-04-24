#define BYTES_PER_PX 3

kernel void BlurNaive(
    const global uchar* source_pixel_data,
    global uchar* dest_pixel_data,
    const int width,
    const int height){
    
    // pixel row&col id
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

constant sampler_t image_sampler=
    CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_NONE|CLK_FILTER_NEAREST;

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
