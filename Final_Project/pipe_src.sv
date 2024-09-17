module pipe_src
#(
    parameter CD = 12,      // color depth
              ADDR = 15,    // number of address bits
              KEY_COLOR =0  // chroma key
   )
   (
    input  logic clk,
    input  logic [10:0] x, y,   // x-and  y-coordinate    
    input  logic [10:0] x0, y0, // origin of sprite 
    input  logic [4:0] ctrl,    // sprite control 
    // sprite ram write
    input  logic we ,
    input  logic [ADDR-1:0] addr_w,
    input  logic [2:0] pixel_in,
    // pixel output
    output logic [CD-1:0] sprite_rgb
   );
   
   // localparam declaration
   localparam H_SIZE = 64; // horizontal size of sprite
   localparam V_SIZE = 480; // vertical size of sprite
   // signal delaration
   logic signed [11:0] xr, yr;  // relative x/y position
   logic in_region;
   logic [ADDR-1:0] addr_r;
   logic [3:0] plt_code;        
   logic [10:0] x_d1_reg;
   logic [CD-1:0]  out_rgb;
   logic [CD-1:0] full_rgb, cut1LG, cut1G, cut1DG, 
                            cut2LG, cut2G, cut2DG, 
                            cut3LG, cut3G, cut3DG,
                            cut4LG, cut4G, cut4DG;
   logic [CD-1:0] out_rgb_d1_reg, bird_rgb;       
   logic [1:0] pipe_cut_sel;               
   
   // body 
   assign pipe_cut_sel = ctrl[1:0];

   //******************************************************************
   // sprite RAM
   //******************************************************************
   // instantiate sprite RAM
   pipe_ram #(.ADDR_WIDTH(ADDR), .DATA_WIDTH(4)) ram_unit (
      .clk(clk), .we(we), .addr_w(addr_w), .din(pixel_in),
      .addr_r(addr_r), .dout(plt_code));
   assign addr_r = {yr[8:0], xr[5:0]};
 
   //******************************************************************
   // ghost color control
   //******************************************************************
   // ghost color selection
   always_comb
      case (pipe_cut_sel)
         2'b00:   begin
                    cut1LG = KEY_COLOR;
                    cut1G = KEY_COLOR;
                    cut1DG = KEY_COLOR;
                    
                    cut2LG = 12'h9E9;
                    cut2G = 12'h0E0;
                    cut2DG = 12'h060;
                    
                    cut3LG = 12'h9E9;
                    cut3G = 12'h0E0;
                    cut3DG = 12'h060;
                    
                    cut4LG = 12'h9E9;
                    cut4G = 12'h0E0;
                    cut4DG = 12'h060;
                  end
         2'b01:   begin
                    cut1LG = 12'h9E9;
                    cut1G = 12'h0E0;
                    cut1DG = 12'h060;
                    
                    cut2LG = KEY_COLOR;
                    cut2G = KEY_COLOR;
                    cut2DG = KEY_COLOR;
                    
                    cut3LG = 12'h9E9;
                    cut3G = 12'h0E0;
                    cut3DG = 12'h060;
                    
                    cut4LG = 12'h9E9;
                    cut4G = 12'h0E0;
                    cut4DG = 12'h060;
                  end
         2'b10:   begin
                    cut1LG = 12'h9E9;
                    cut1G = 12'h0E0;
                    cut1DG = 12'h060;
                    
                    cut2LG = 12'h9E9;
                    cut2G = 12'h0E0;
                    cut2DG = 12'h060;
                    
                    cut3LG = KEY_COLOR;
                    cut3G = KEY_COLOR;
                    cut3DG = KEY_COLOR;
                    
                    cut4LG = 12'h9E9;
                    cut4G = 12'h0E0;
                    cut4DG = 12'h060;
                  end
         2'b11:   begin
                    cut1LG = 12'h9E9;
                    cut1G = 12'h0E0;
                    cut1DG = 12'h060;
                    
                    cut2LG = 12'h9E9;
                    cut2G = 12'h0E0;
                    cut2DG = 12'h060;
                    
                    cut3LG = 12'h9E9;
                    cut3G = 12'h0E0;
                    cut3DG = 12'h060;
                    
                    cut4LG = KEY_COLOR;
                    cut4G = KEY_COLOR;
                    cut4DG = KEY_COLOR;
                  end
         default: begin
                    cut1LG = KEY_COLOR;
                    cut1G = KEY_COLOR;
                    cut1DG = KEY_COLOR;
                    
                    cut2LG = 12'h9E9;
                    cut2G = 12'h0E0;
                    cut2DG = 12'h060;
                    
                    cut3LG = 12'h9E9;
                    cut3G = 12'h0E0;
                    cut3DG = 12'h060;
                    
                    cut4LG = 12'h9E9;
                    cut4G = 12'h0E0;
                    cut4DG = 12'h060;
                  end
      endcase   
   // palette table
   always_comb
      case (plt_code)
         4'b0000:   full_rgb = 12'h9E9;   // light green key
         4'b0001:   full_rgb = 12'h0E0;   // green key
         4'b0010:   full_rgb = 12'h060;   // dark green key
         4'b0011:   full_rgb = 12'h000;   // black key
         4'b0100:   full_rgb = cut1LG;   
         4'b0101:   full_rgb = cut1G;   
         4'b0110:   full_rgb = cut1DG;   
         4'b0111:   full_rgb = cut2LG;   
         4'b1000:   full_rgb = cut2G;   
         4'b1001:   full_rgb = cut2DG;   
         4'b1010:   full_rgb = cut3LG;   
         4'b1011:   full_rgb = cut3G;   
         4'b1100:   full_rgb = cut3DG;   
         4'b1101:   full_rgb = cut4LG;   
         4'b1110:   full_rgb = cut4G;   
         4'b1111:   full_rgb = cut4DG;   
         
      endcase   
   //******************************************************************
   // in-region circuit
   //******************************************************************
   // relative coordinate calculation
   assign xr = $signed({1'b0, x}) - $signed({1'b0, x0});
   assign yr = $signed({1'b0, y}) - $signed({1'b0, y0});
   // in-region comparison and multiplexing 
   assign in_region = ((0<= xr) && (xr<H_SIZE) && (0<=yr) && (yr<V_SIZE));
   assign out_rgb = in_region ? full_rgb : KEY_COLOR;
//   //******************************************************************
//   // animation timing control
//   //******************************************************************
//   // counters 
//   always_ff @(posedge clk) begin
//      x_d1_reg <= x;
//      c_reg <= c_next;
//      ani_reg <= ani_next;
//   end
//   assign c_next = (frame_tick && c_reg==9) ? 0 :
//                   (frame_tick) ? c_reg + 1 :
//                    c_reg; 
//   assign ani_next = (ani_tick) ? ani_reg + 1 : ani_reg;
//   // 60-Hz tick from fram counter 
//   assign frame_tick = (x_d1_reg==0) && (x==1) && (y==0);
//   // sprite animation id tick 
//   assign ani_tick  = frame_tick  && (c_reg==0); 
//   // sprite id selection
//   assign sid = (auto) ? ani_reg : gc_id_sel;
//   //******************************************************************
//   // delay line (one clock) 
//   //******************************************************************
   // output with a-stage delay line
   always_ff @(posedge clk) 
      out_rgb_d1_reg <= out_rgb;
   assign sprite_rgb = out_rgb_d1_reg;
endmodule
