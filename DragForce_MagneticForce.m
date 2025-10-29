%% 
close all
clear all

% aaa = 3400;
% bbb = 5000;
% ccc = 3400;
% ddd = 5000;

aaa = 4770; %10Hz PI
bbb = 5730;
ccc = 4770;
ddd = 5730;

% aaa = 5370; %10Hz PI
% bbb = 6330;
% ccc = 5370;
% ddd = 6330;

% aaa = 2684; %30Hz 60Hz PI
% bbb = 3281;
% ccc = 2684;
% ddd = 3281;

% aaa = 4462;
% bbb = 5424;
% ccc = 4462;
% ddd = 5424;

% aaa = 2494;
% bbb = 3435;
% ccc = 2494;
% ddd = 3435;
% gz = 0.04;
kb = 1.38064852*10^-23;
T = 294;
% gamma = 8*10^-6;
gamma = 4*10^-8;
delta_t = 1/1600;
Ft = 4*T*delta_t*kb/gamma;
Thm_power_x = 4*gamma*kb*T; %single sided
Brownian_motion = 4*kb*delta_t*T/gamma;

str='3Dsine_water_newouter_L088_g004_A1_10Hz_30cycle_4_2_lamf086_lamci082_lamei08';

file=[str '.dat'];
u=load(file);

mag_z=100;
mag_xy=8000/40;
rate=1600;

length2=size(u);
data_num=length2(1)-1;
ch_num=length2(2);

index=u(1,:);

for i=1:ch_num
    Data_array(:,index(i)+1)=u(2:data_num+1,i);
end
%% 



t=Data_array(:,1)/rate;
% t=linspace(0,7.181/2,11500);
cnt=Data_array(:,2);
x1=Data_array(:,3)*mag_xy;
y1=Data_array(:,4)*mag_xy;
z1=Data_array(:,5)*mag_z;
x2=Data_array(:,6)*mag_xy;
y2=Data_array(:,7)*mag_xy;
z2=Data_array(:,8)*mag_z;

hs_cnt=Data_array(:,9);
HsVm_ch1=Data_array(:,10);
HsVm_ch2=Data_array(:,11);
HsVm_ch3=Data_array(:,12);
HsVm_ch4=Data_array(:,13);
HsVm_ch5=Data_array(:,14);
HsVm_ch6=Data_array(:,15);
HsVd_ch1=Data_array(:,16);
HsVd_ch2=Data_array(:,17);
HsVd_ch3=Data_array(:,18);
HsVd_ch4=Data_array(:,19);
HsVd_ch5=Data_array(:,20);
HsVd_ch6=Data_array(:,21);

% pio_rx=Data_array(:,22);
% pio_ry=Data_array(:,23);
% pio_rz=Data_array(:,24);
% pio_ax=Data_array(:,25);
% pio_ay=Data_array(:,26);
% pio_az=Data_array(:,27);

Vm_16b_ch1=Data_array(:,28);
Vm_16b_ch2=Data_array(:,29);
Vm_16b_ch3=Data_array(:,30);
Vm_16b_ch4=Data_array(:,31);
Vm_16b_ch5=Data_array(:,32);
Vm_16b_ch6=Data_array(:,33);
Vm_16b_ch7=Data_array(:,34);
Vm_16b_ch8=Data_array(:,35);
Vm_16b_ch9=Data_array(:,36);
Vm_16b_ch10=Data_array(:,37);
Vm_16b_ch11=Data_array(:,38);
Vm_16b_ch12=Data_array(:,39);


DA_ch1=Data_array(:,40);
DA_ch2=Data_array(:,41);
DA_ch3=Data_array(:,42);
DA_ch4=Data_array(:,43);
DA_ch5=Data_array(:,44);
DA_ch6=Data_array(:,45);


DA_ch1=(DA_ch1-32767)*3/32768;
DA_ch2=(DA_ch2-32767)*3/32768;
DA_ch3=(DA_ch3-32767)*3/32768;
DA_ch4=(DA_ch4-32767)*3/32768;
DA_ch5=(DA_ch5-32767)*3/32768;
DA_ch6=(DA_ch6-32767)*3/32768;

% B1M_raw_x=Data_array(:,51)*1000;
% B1M_raw_y=Data_array(:,52)*1000;
% B1M_raw_z=Data_array(:,53)*1000;
% B1M_cmp_x=Data_array(:,54)*1000;
% B1M_cmp_y=Data_array(:,55)*1000;
% B1M_cmp_z=Data_array(:,56)*1000;
B1M_raw_x=Data_array(:,51);
B1M_raw_y=Data_array(:,52);
B1M_raw_z=Data_array(:,53);
B1M_cmp_x=Data_array(:,54);
B1M_cmp_y=Data_array(:,55);
B1M_cmp_z=Data_array(:,56);


B1T_raw_x=Data_array(:,57);
B1T_raw_y=Data_array(:,58);
B1T_raw_z=Data_array(:,59);
B1T_cmp_x=Data_array(:,60);
B1T_cmp_y=Data_array(:,61);
B1T_cmp_z=Data_array(:,62);

I_ch1 = Data_array(:,69);
I_ch2 = Data_array(:,70);
I_ch3 = Data_array(:,71);
I_ch4 = Data_array(:,72);
I_ch5 = Data_array(:,73);
I_ch6 = Data_array(:,74);

FD_x_om = Data_array(:,75);
FD_y_om = Data_array(:,76);
FD_z_om = Data_array(:,77);

FD_x_oa = Data_array(:,78);
FD_y_oa = Data_array(:,79);
FD_z_oa = Data_array(:,80);

FD_x_om_new = Data_array(:,87);
FD_y_om_new = Data_array(:,88);
FD_z_om_new = Data_array(:,89);

% ax = Data_array(:,90);
% ay = Data_array(:,91);
% az = Data_array(:,92);
% 
% Kx = Data_array(:,93);
% Ky = Data_array(:,94);
% Kz = Data_array(:,95);
Dx_FPGA = Data_array(:,90);
Dy_FPGA = Data_array(:,91);
Dz_FPGA = Data_array(:,92);

gxy_FPGA = Data_array(:,105);
gz_FPGA = Data_array(:,106);
% Total_Dx = Data_array(:,105);
% Total_Dy = Data_array(:,106);
Total_Dz = Data_array(:,107);
% Pzk1 = Data_array(:,107);


% hs_dac_a_ch1=Data_array(:,93);
% hs_dac_a_ch2=Data_array(:,94);
% hs_dac_a_ch3=Data_array(:,95);
% hs_dac_a_ch4=Data_array(:,96);
% hs_dac_a_ch5=Data_array(:,97);
% hs_dac_a_ch6=Data_array(:,98);
% 
% hs_dac_b_ch1=Data_array(:,99);
% hs_dac_b_ch2=Data_array(:,100);
% hs_dac_b_ch3=Data_array(:,101);
% hs_dac_b_ch4=Data_array(:,102);
% hs_dac_b_ch5=Data_array(:,103);
% hs_dac_b_ch6=Data_array(:,104);

%% 
R_norm = 600;
id_a2m(1,1) = -1.0/sqrt(6.0);  id_a2m(1,2) = -1.0/sqrt(6.0);  id_a2m(1,3) = -sqrt(2.0/3.0);
id_a2m(2,1) =  1.0/sqrt(2.0);  id_a2m(2,2) = -1.0/sqrt(2.0);  id_a2m(2,3) = 0.0;
id_a2m(3,1) = -1.0/sqrt(3.0);  id_a2m(3,2) = -1.0/sqrt(3.0);  id_a2m(3,3) = 1.0/sqrt(3.0);
% id_m2a = inv(id_a2m);
id_m2a = id_a2m';
Fvgain = 8;
iD = [2.2523749 0 0 0 0 0;
    0 2.7488400 0 0 0 0;
    0 0 2.2456229 0 0 0;
    0 0 0 2.2936800 0 0;
    0 0 0 0 2.8320400 0;
    0 0 0 0 0 2.2947314];
D = inv(iD);
KI = 1/6*[5 -1 -1 -1 -1 -1;
    -1 5 -1 -1 -1 -1;
    -1 -1 5 -1 -1 -1;
    -1 -1 -1 5 -1 -1;
    -1 -1 -1 -1 5 -1;
    -1 -1 -1 -1 -1 5];
% D = D*KI;
% D = [1 0 0 0 0 0;
%     0 0.699846 0 0 0 0;
%     0 0 1.004841 0 0 0;
%     0 0 0 0.890481 0 0;
%     0 0 0 0 0.941824 0;
%     0 0 0 0 0 1.129175];
for i = 1:18
    Bias_umOa_P1toP6(i) = 0.0;
%     Bias_umOm_P1toP6(1) = -172.807717;
%     Bias_umOm_P1toP6(2) = 381.127619;
%     Bias_umOm_P1toP6(3) = 3.994553;
%     Bias_umOm_P1toP6(4) = -96.702438;
%     Bias_umOm_P1toP6(5) = -59.850604;
%     Bias_umOm_P1toP6(6) = 234.847287;
%     Bias_umOm_P1toP6(7) = -71.096759;
%     Bias_umOm_P1toP6(8) = -257.971293;
%     Bias_umOm_P1toP6(9) = -62.871233;
%     Bias_umOm_P1toP6(10) = -100.335609;
%     Bias_umOm_P1toP6(11) = 211.057887;
%     Bias_umOm_P1toP6(12) = 301.695055;
%     Bias_umOm_P1toP6(13) = -215.039561;
%     Bias_umOm_P1toP6(14) = 18.081206;
%     Bias_umOm_P1toP6(15) = 310.293750;
%     Bias_umOm_P1toP6(16) = 294.702900;
%     Bias_umOm_P1toP6(17) = -3.777273;
%     Bias_umOm_P1toP6(18) = -119.227428;
end
% for i = 1:3
%     Bias_umOm_P1(i) = Bias_umOm_P1toP6(i);
%     Bias_umOm_P2(i) = Bias_umOm_P1toP6(i+3);
%     Bias_umOm_P3(i) = Bias_umOm_P1toP6(i+6);
%     Bias_umOm_P4(i) = Bias_umOm_P1toP6(i+9);
%     Bias_umOm_P5(i) = Bias_umOm_P1toP6(i+12);
%     Bias_umOm_P6(i) = Bias_umOm_P1toP6(i+15);
% end
% Bias_umOa_P1 = id_m2a*Bias_umOm_P1';
% Bias_umOa_P2 = id_m2a*Bias_umOm_P2';
% Bias_umOa_P3 = id_m2a*Bias_umOm_P3';
% Bias_umOa_P4 = id_m2a*Bias_umOm_P4';
% Bias_umOa_P5 = id_m2a*Bias_umOm_P5';
% Bias_umOa_P6 = id_m2a*Bias_umOm_P6';
% 
% 
% Bias_umOa_P1toP6 = [Bias_umOa_P1;Bias_umOa_P2;Bias_umOa_P3;Bias_umOa_P4;Bias_umOa_P5;Bias_umOa_P6];


FFF = 50;
for i = 1:length(HsVm_ch1)
     if i<=FFF/2
         mean_running_HsVm_ch1(i) = sum(HsVm_ch1(i:i+FFF-1))/FFF;
         mean_running_HsVm_ch2(i) = sum(HsVm_ch2(i:i+FFF-1))/FFF;
         mean_running_HsVm_ch3(i) = sum(HsVm_ch3(i:i+FFF-1))/FFF;
         mean_running_HsVm_ch4(i) = sum(HsVm_ch4(i:i+FFF-1))/FFF;
         mean_running_HsVm_ch5(i) = sum(HsVm_ch5(i:i+FFF-1))/FFF;
         mean_running_HsVm_ch6(i) = sum(HsVm_ch6(i:i+FFF-1))/FFF;

     elseif i>=length(HsVm_ch1)-FFF/2
         mean_running_HsVm_ch1(i) = sum(HsVm_ch1(i-FFF-1:i))/FFF;
         mean_running_HsVm_ch2(i) = sum(HsVm_ch2(i-FFF-1:i))/FFF;
         mean_running_HsVm_ch3(i) = sum(HsVm_ch3(i-FFF-1:i))/FFF;
         mean_running_HsVm_ch4(i) = sum(HsVm_ch4(i-FFF-1:i))/FFF;
         mean_running_HsVm_ch5(i) = sum(HsVm_ch5(i-FFF-1:i))/FFF;
         mean_running_HsVm_ch6(i) = sum(HsVm_ch6(i-FFF-1:i))/FFF;
     else
         mean_running_HsVm_ch1(i) = sum(HsVm_ch1(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_HsVm_ch2(i) = sum(HsVm_ch2(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_HsVm_ch3(i) = sum(HsVm_ch3(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_HsVm_ch4(i) = sum(HsVm_ch4(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_HsVm_ch5(i) = sum(HsVm_ch5(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_HsVm_ch6(i) = sum(HsVm_ch6(i-FFF/2:i+FFF/2))/(FFF+1);
     end
end
HsVm1_random = HsVm_ch1-mean_running_HsVm_ch1';
HsVm2_random = HsVm_ch2-mean_running_HsVm_ch2';
HsVm3_random = HsVm_ch3-mean_running_HsVm_ch3';
HsVm4_random = HsVm_ch4-mean_running_HsVm_ch4';
HsVm5_random = HsVm_ch5-mean_running_HsVm_ch5';
HsVm6_random = HsVm_ch6-mean_running_HsVm_ch6';



for i = 1:length(B1M_raw_x)
    Pos_umOm = [B1M_raw_x(i);B1M_raw_y(i);B1M_raw_z(i)];
    Pos_umOa = id_m2a*Pos_umOm;
    x = Pos_umOa(1)/R_norm;                                 %norlizing
    y = Pos_umOa(2)/R_norm;                                 %norlizing
    z = Pos_umOa(3)/R_norm;                                 %norlizing

    Dx1_x = Bias_umOa_P1toP6(1)/R_norm  - x;
    Dx2_x = Bias_umOa_P1toP6(4)/R_norm  - x;
    Dx3_x = Bias_umOa_P1toP6(7)/R_norm  - x;
    Dx4_x = Bias_umOa_P1toP6(10)/R_norm  - x;
    Dx5_x = Bias_umOa_P1toP6(11)/R_norm - x;
    Dx6_x = Bias_umOa_P1toP6(16)/R_norm - x;

    Dy1_y = Bias_umOa_P1toP6(2)/R_norm  - y;
    Dy2_y = Bias_umOa_P1toP6(5)/R_norm  - y;
    Dy3_y = Bias_umOa_P1toP6(8)/R_norm  - y;
    Dy4_y = Bias_umOa_P1toP6(11)/R_norm - y;
    Dy5_y = Bias_umOa_P1toP6(14)/R_norm - y;
    Dy6_y = Bias_umOa_P1toP6(17)/R_norm - y;

    Dz1_z = Bias_umOa_P1toP6(3)/R_norm  - z;
    Dz2_z = Bias_umOa_P1toP6(6)/R_norm  - z;
    Dz3_z = Bias_umOa_P1toP6(9)/R_norm  - z;
    Dz4_z = Bias_umOa_P1toP6(12)/R_norm - z; 
    Dz5_z = Bias_umOa_P1toP6(15)/R_norm - z; 
    Dz6_z = Bias_umOa_P1toP6(18)/R_norm - z;

    Dx1_xPlus1 = Bias_umOa_P1toP6(1)/R_norm - x + 1;
    x_Dx2Plus1 = x - Bias_umOa_P1toP6(4)/R_norm + 1;
    Dy3_yPlus1 = Bias_umOa_P1toP6(8)/R_norm - y + 1;
    y_Dy4Plus1 = y - Bias_umOa_P1toP6(11)/R_norm + 1;
    Dz5_zPlus1 = Bias_umOa_P1toP6(15)/R_norm - z + 1;
    z_Dz6Plus1 = z - Bias_umOa_P1toP6(18)/R_norm + 1;

    Sq_r1 = (Dx1_xPlus1^2 + Dy1_y^2 + Dz1_z^2);
    Sq_r2 = (x_Dx2Plus1^2 + Dy2_y^2 + Dz2_z^2);
    Sq_r3 = (Dx3_x^2 + Dy3_yPlus1^2 + Dz3_z^2);
    Sq_r4 = (Dx4_x^2 + y_Dy4Plus1^2 + Dz4_z^2);
    Sq_r5 = (Dx5_x^2 + Dy5_y^2 + Dz5_zPlus1^2);
    Sq_r6 = (Dx6_x^2 + Dy6_y^2 + z_Dz6Plus1^2); 


    Lx(1,1) = 4*Dx1_xPlus1/(Sq_r1^3);
    Lx(1,2) = (((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1)*Dx1_xPlus1 - ((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1)*x_Dx2Plus1)/((Sq_r1^1.5)*(Sq_r2^1.5));
    Lx(1,3) = (((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1)*Dx1_xPlus1 + Dx3_x*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1))/((Sq_r1^1.5)*(Sq_r3^1.5));
    Lx(1,4) = (((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1)*Dx1_xPlus1 + Dx4_x*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1))/((Sq_r1^1.5)*(Sq_r4^1.5));
    Lx(1,5) = (((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1)*Dx1_xPlus1 + Dx5_x*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r1^1.5)*(Sq_r5^1.5));
    Lx(1,6) = (((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1)*Dx1_xPlus1 + Dx6_x*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r1^1.5)*(Sq_r6^1.5));

    Lx(2,1) =  Lx(1,2);
    Lx(2,2) = -4*x_Dx2Plus1/(Sq_r2^3);
    Lx(2,3) = -(((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1)*x_Dx2Plus1 - Dx3_x*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1))/((Sq_r2^1.5)*(Sq_r3^1.5));
    Lx(2,4) =  (((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1)*x_Dx2Plus1 - Dx4_x*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1))/((Sq_r2^1.5)*(Sq_r4^1.5));
    Lx(2,5) = -(((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1)*x_Dx2Plus1 - Dx5_x*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r2^1.5)*(Sq_r5^1.5));
    Lx(2,6) =  (((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1)*x_Dx2Plus1 - Dx6_x*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1))/((Sq_r2^1.5)*(Sq_r6^1.5));

    Lx(3,1) = Lx(1,3);
    Lx(3,2) = Lx(2,3);
    Lx(3,3) = 4*Dx3_x/(Sq_r3^3);
    Lx(3,4) = (Dx3_x*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1) + Dx4_x*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1))/((Sq_r3^1.5)*(Sq_r4^1.5));
    Lx(3,5) = (Dx3_x*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1) + Dx5_x*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r3^1.5)*(Sq_r5^1.5));
    Lx(3,6) = (Dx3_x*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1) + Dx6_x*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r3^1.5)*(Sq_r6^1.5));

    Lx(4,1) =  Lx(1,4);
    Lx(4,2) =  Lx(2,4);
    Lx(4,3) =  Lx(3,4);
    Lx(4,4) =  4*Dx4_x/(Sq_r4^3);
    Lx(4,5) =  (Dx4_x*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1) + Dx5_x*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r4^1.5)*(Sq_r5^1.5));
    Lx(4,6) = -(Dx4_x*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1) + Dx6_x*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1))/((Sq_r4^1.5)*(Sq_r6^1.5));

    Lx(5,1) = Lx(1,5);
    Lx(5,2) = Lx(2,5);
    Lx(5,3) = Lx(3,5);
    Lx(5,4) = Lx(4,5);
    Lx(5,5) = 4*Dx5_x/(Sq_r5^3);
    Lx(5,6) = (Dx5_x*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1) + Dx6_x*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r5^1.5)*(Sq_r6^1.5));

    Lx(6,1) = Lx(1,6);
    Lx(6,2) = Lx(2,6);
    Lx(6,3) = Lx(3,6);
    Lx(6,4) = Lx(4,6);
    Lx(6,5) = Lx(5,6);
    Lx(6,6) = 4*Dx6_x/(Sq_r6^3);

    Ly(1,1) = 4*Dy1_y/(Sq_r1^3);
    Ly(1,2) = (Dy1_y*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1) + Dy2_y*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1))/((Sq_r1^1.5)*(Sq_r2^1.5));
    Ly(1,3) = (Dy1_y*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1) + ((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1)*Dy3_yPlus1)/((Sq_r1^1.5)*(Sq_r3^1.5));
    Ly(1,4) = (Dy1_y*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1) - ((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1)*y_Dy4Plus1)/((Sq_r1^1.5)*(Sq_r4^1.5));
    Ly(1,5) = (Dy1_y*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1) + Dy5_y*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r1^1.5)*(Sq_r5^1.5));
    Ly(1,6) = (Dy1_y*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1) + Dy6_y*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r1^1.5)*(Sq_r6^1.5));

    Ly(2,1) =  Ly(1,2);
    Ly(2,2) =  4*Dy2_y/(Sq_r2^3);
    Ly(2,3) =  (Dy2_y*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1) + ((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1)*Dy3_yPlus1)/((Sq_r2^1.5)*(Sq_r3^1.5));
    Ly(2,4) = -(Dy2_y*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1) - ((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1)*y_Dy4Plus1)/((Sq_r2^1.5)*(Sq_r4^1.5));
    Ly(2,5) =  (Dy2_y*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1) + Dy5_y*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r2^1.5)*(Sq_r5^1.5));
    Ly(2,6) = -(Dy2_y*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1) + Dy6_y*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1))/((Sq_r2^1.5)*(Sq_r6^1.5));

    Ly(3,1) = Ly(1,3);
    Ly(3,2) = Ly(2,3);
    Ly(3,3) = 4*Dy3_yPlus1/(Sq_r3^3);
    Ly(3,4) = (((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1)*Dy3_yPlus1 - ((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1)*y_Dy4Plus1)/((Sq_r3^1.5)*(Sq_r4^1.5));
    Ly(3,5) = (((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1)*Dy3_yPlus1 + Dy5_y*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r3^1.5)*(Sq_r5^1.5));
    Ly(3,6) = (((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1)*Dy3_yPlus1 + Dy6_y*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r3^1.5)*(Sq_r6^1.5));

    Ly(4,1) =  Ly(1,4);
    Ly(4,2) =  Ly(2,4);
    Ly(4,3) =  Ly(3,4);
    Ly(4,4) = -4*y_Dy4Plus1/(Sq_r4^3);
    Ly(4,5) = -(((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1)*y_Dy4Plus1 - Dy5_y*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1))/((Sq_r4^1.5)*(Sq_r5^1.5));
    Ly(4,6) =  (((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1)*y_Dy4Plus1 - Dy6_y*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1))/((Sq_r4^1.5)*(Sq_r6^1.5));

    Ly(5,1) = Ly(1,5);
    Ly(5,2) = Ly(2,5);
    Ly(5,3) = Ly(3,5);
    Ly(5,4) = Ly(4,5);
    Ly(5,5) = 4*Dy5_y/(Sq_r5^3);
    Ly(5,6) = (Dy5_y*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1) + Dy6_y*((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1))/((Sq_r5^1.5)*(Sq_r6^1.5));

    Ly(6,1) = Ly(1,6);
    Ly(6,2) = Ly(2,6);
    Ly(6,3) = Ly(3,6);
    Ly(6,4) = Ly(4,6);
    Ly(6,5) = Ly(5,6);
    Ly(6,6) = 4*Dy6_y/(Sq_r6^3);

    Lz(1,1) = 4*Dz1_z/(Sq_r1^3);
    Lz(1,2) = (Dz1_z*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r1 - 1) + Dz2_z*((3*Dy1_y*Dy2_y - 3*Dx1_xPlus1*x_Dx2Plus1 + 3*Dz1_z*Dz2_z)/Sq_r2 - 1))/((Sq_r1^1.5)*(Sq_r2^1.5));
    Lz(1,3) = (Dz1_z*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r1 - 1) + Dz3_z*((3*Dx3_x*Dx1_xPlus1 + 3*Dy1_y*Dy3_yPlus1 + 3*Dz1_z*Dz3_z)/Sq_r3 - 1))/((Sq_r1^1.5)*(Sq_r3^1.5));
    Lz(1,4) = (Dz1_z*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r1 - 1) + Dz4_z*((3*Dx4_x*Dx1_xPlus1 - 3*Dy1_y*y_Dy4Plus1 + 3*Dz1_z*Dz4_z)/Sq_r4 - 1))/((Sq_r1^1.5)*(Sq_r4^1.5));
    Lz(1,5) = (Dz1_z*((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r1 - 1) + ((3*Dx5_x*Dx1_xPlus1 + 3*Dy1_y*Dy5_y + 3*Dz1_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/((Sq_r1^1.5)*(Sq_r5^1.5));
    Lz(1,6) = (Dz1_z*((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r1 - 1) - ((3*Dx6_x*Dx1_xPlus1 + 3*Dy1_y*Dy6_y - 3*Dz1_z*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/((Sq_r1^1.5)*(Sq_r6^1.5));

    Lz(2,1) =  Lz(1,2);
    Lz(2,2) =  4*Dz2_z/(Sq_r2^3);
    Lz(2,3) =  (Dz2_z*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r2 - 1) + Dz3_z*((3*Dy2_y*Dy3_yPlus1 - 3*Dx3_x*x_Dx2Plus1 + 3*Dz2_z*Dz3_z)/Sq_r3 - 1))/((Sq_r2^1.5)*(Sq_r3^1.5));
    Lz(2,4) = -(Dz2_z*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r2 + 1) + Dz4_z*((3*Dx4_x*x_Dx2Plus1 + 3*Dy2_y*y_Dy4Plus1 - 3*Dz2_z*Dz4_z)/Sq_r4 + 1))/((Sq_r2^1.5)*(Sq_r4^1.5));
    Lz(2,5) =  (Dz2_z*((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r2 - 1) + ((3*Dy2_y*Dy5_y - 3*Dx5_x*x_Dx2Plus1 + 3*Dz2_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/((Sq_r2^1.5)*(Sq_r5^1.5));
    Lz(2,6) = -(Dz2_z*((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r2 + 1) - ((3*Dx6_x*x_Dx2Plus1 - 3*Dy2_y*Dy6_y + 3*Dz2_z*z_Dz6Plus1)/Sq_r6 + 1)*z_Dz6Plus1)/((Sq_r2^1.5)*(Sq_r6^1.5));

    Lz(3,1) = Lz(1,3);
    Lz(3,2) = Lz(2,3);
    Lz(3,3) = 4*Dz3_z/(Sq_r3^3);
    Lz(3,4) = (Dz3_z*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r3 - 1) + Dz4_z*((3*Dx3_x*Dx4_x - 3*Dy3_yPlus1*y_Dy4Plus1 + 3*Dz3_z*Dz4_z)/Sq_r4 - 1))/((Sq_r3^1.5)*(Sq_r4^1.5));
    Lz(3,5) = (Dz3_z*((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r3 - 1) + ((3*Dx3_x*Dx5_x + 3*Dy5_y*Dy3_yPlus1 + 3*Dz3_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/((Sq_r3^1.5)*(Sq_r5^1.5));
    Lz(3,6) = (Dz3_z*((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r3 - 1) - ((3*Dx3_x*Dx6_x + 3*Dy6_y*Dy3_yPlus1 - 3*Dz3_z*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/((Sq_r3^1.5)*(Sq_r6^1.5));

    Lz(4,1) =  Lz(1,4);
    Lz(4,2) =  Lz(2,4);
    Lz(4,3) =  Lz(3,4);
    Lz(4,4) =  4*Dz4_z/(Sq_r4^3);
    Lz(4,5) =  (Dz4_z*((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r4 - 1) + ((3*Dx4_x*Dx5_x - 3*Dy5_y*y_Dy4Plus1 + 3*Dz4_z*Dz5_zPlus1)/Sq_r5 - 1)*Dz5_zPlus1)/((Sq_r4^1.5)*(Sq_r5^1.5));
    Lz(4,6) = -(Dz4_z*((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r4 + 1) - ((3*Dy6_y*y_Dy4Plus1 - 3*Dx4_x*Dx6_x + 3*Dz4_z*z_Dz6Plus1)/Sq_r6 + 1)*z_Dz6Plus1)/((Sq_r4^1.5)*(Sq_r6^1.5));

    Lz(5,1) = Lz(1,5);
    Lz(5,2) = Lz(2,5);
    Lz(5,3) = Lz(3,5);
    Lz(5,4) = Lz(4,5);
    Lz(5,5) = 4*Dz5_zPlus1/(Sq_r5^3);
    Lz(5,6) = (((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r5 - 1)*Dz5_zPlus1 - ((3*Dx5_x*Dx6_x + 3*Dy5_y*Dy6_y - 3*Dz5_zPlus1*z_Dz6Plus1)/Sq_r6 - 1)*z_Dz6Plus1)/((Sq_r5^1.5)*(Sq_r6^1.5));

    Lz(6,1) =  Lz(1,6);
    Lz(6,2) =  Lz(2,6);
    Lz(6,3) =  Lz(3,6);
    Lz(6,4) =  Lz(4,6);
    Lz(6,5) =  Lz(5,6);
    Lz(6,6) = -4*z_Dz6Plus1/(Sq_r6^3);
    
    Vm = [HsVm_ch1(i);HsVm_ch2(i);HsVm_ch3(i);HsVm_ch4(i);HsVm_ch5(i);HsVm_ch6(i)];
    Vm_running = [mean_running_HsVm_ch1(i);mean_running_HsVm_ch2(i);mean_running_HsVm_ch3(i);mean_running_HsVm_ch4(i);mean_running_HsVm_ch5(i);mean_running_HsVm_ch6(i)];
%     I  = [I_ch1(i);I_ch2(i);I_ch3(i);I_ch4(i);I_ch5(i);I_ch6(i)];
    I  = [DA_ch1(i);DA_ch2(i);DA_ch3(i);DA_ch4(i);DA_ch5(i);DA_ch6(i)];
    Fx_Oa(i) = Fvgain*Vm'*D'*Lx*D*Vm;
    Fy_Oa(i) = Fvgain*Vm'*D'*Ly*D*Vm;
    Fz_Oa(i) = Fvgain*Vm'*D'*Lz*D*Vm;
    
%     Fx_Oa(i) = 4.74*Vm'*D'*Lx*D*Vm;
%     Fy_Oa(i) = 4.74*Vm'*D'*Ly*D*Vm;
%     Fz_Oa(i) = 4.74*Vm'*D'*Lz*D*Vm;
    
    Fx_running_Oa(i) = Fvgain*Vm_running'*D'*Lx*D*Vm_running;
    Fy_running_Oa(i) = Fvgain*Vm_running'*D'*Ly*D*Vm_running;
    Fz_running_Oa(i) = Fvgain*Vm_running'*D'*Lz*D*Vm_running;
    Fx_current_Oa(i) = Fvgain*I'*KI'*Lx*KI*I;
    Fy_current_Oa(i) = Fvgain*I'*KI'*Ly*KI*I;
    Fz_current_Oa(i) = Fvgain*I'*KI'*Lz*KI*I;
    Fmag_Oa = [Fx_Oa(i);Fy_Oa(i);Fz_Oa(i)];
    Fmag_current_Oa = [Fx_current_Oa(i);Fy_current_Oa(i);Fz_current_Oa(i)];
    Fmag_running_Oa = [Fx_running_Oa(i);Fy_running_Oa(i);Fz_running_Oa(i)];
    Fmag_Om = id_a2m*Fmag_Oa;
    Fmag_current_Om = id_a2m*Fmag_current_Oa;
    Fmag_running_Om = id_a2m*Fmag_running_Oa;
    Fx_Om(i) = Fmag_Om(1);      %voltage 2.3/1.6*  2.15/1.6*
    Fy_Om(i) = Fmag_Om(2);               %voltage 1.262/1.762*
    Fz_Om(i) = Fmag_Om(3);
    Fx_current_Om(i) = Fmag_current_Om(1);
    Fy_current_Om(i) = Fmag_current_Om(2);
    Fz_current_Om(i) = Fmag_current_Om(3);
    Fx_running_Om(i) = Fmag_running_Om(1);
    Fy_running_Om(i) = Fmag_running_Om(2);
    Fz_running_Om(i) = Fmag_running_Om(3);
end
for i = 1:length(B1M_raw_x)
    if i==1
        deltax(i) = 0;
        deltay(i) = 0;
        deltaz(i) = 0;
    else
        deltax(i) = 1600*(B1M_raw_x(i)-B1M_raw_x(i-1));
        deltay(i) = 1600*(B1M_raw_y(i)-B1M_raw_y(i-1));
        deltaz(i) = 1600*(B1M_raw_z(i)-B1M_raw_z(i-1));
        
    end
end


for i = 1:length(deltax)
     if i<=FFF/2
     mean_running_deltax(i) = sum(deltax(i:i+FFF-1))/FFF;
     mean_running_deltay(i) = sum(deltay(i:i+FFF-1))/FFF;
     mean_running_deltaz(i) = sum(deltaz(i:i+FFF-1))/FFF;

     elseif i>=length(deltax)-FFF/2
           mean_running_deltax(i) = sum(deltax(i-FFF-1:i))/FFF;
            mean_running_deltay(i) = sum(deltay(i-FFF-1:i))/FFF;
            mean_running_deltaz(i) = sum(deltaz(i-FFF-1:i))/FFF;
     else
         mean_running_deltax(i) = sum(deltax(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_deltay(i) = sum(deltay(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_deltaz(i) = sum(deltaz(i-FFF/2:i+FFF/2))/(FFF+1);
     end
     
    gnyx = 0.04;
    gnz = 0.04;  %voltage 0.045
    R = 2.25;
    h = B1M_raw_z(i);
    ratio = R/h;
    gx(i) = gnyx/(1-9/16*ratio+1/8*ratio^3-45/256*ratio^4-1/16*ratio^5);
    gy(i) = gnyx/(1-9/16*ratio+1/8*ratio^3-45/256*ratio^4-1/16*ratio^5);
    gz(i) = gnz/(1-9/8*ratio+1/2*ratio^3-57/100*ratio^4+1/5*ratio^5+7/200*ratio^11-1/25*ratio^12);
    
    gy_temp(i) = gnyx/(1-9/16*ratio+1/8*ratio^3);
    gz_temp(i) = gnz/(1-9/8*ratio+1/2*ratio^3);

end
Thm_power_x = 4*gx*10^-6*kb*T/delta_t; %single sided
Thm_power_y = 4*gy*10^-6*kb*T/delta_t; %single sided
Thm_power_z = 4*gz*10^-6*kb*T/delta_t; %single sided


gamma = 0.04;
T = 1/1600;
lam = 0.9;
for i = 1:length(B1M_raw_x)
    if i==1
        B1T_raw_x_d1(i) = 0;
        B1T_raw_y_d1(i) = 0;
        B1T_raw_z_d1(i) = 0;
                B1T_raw_x_d2(i) = 0;
        B1T_raw_y_d2(i) = 0;
        B1T_raw_z_d2(i) = 0;
                B1T_raw_x_d3(i) = 0;
        B1T_raw_y_d3(i) = 0;
        B1T_raw_z_d3(i) = 0;
        usim_k1(i) = FD_x_om_new(i);
        usim_k2(i) = FD_x_om_new(i);
        usim_k3(i) = FD_x_om_new(i);
    end
    usim(i) = 2*((gamma/T)*(B1T_raw_x(i)-lam*B1T_raw_x_d1(i)-(1-lam)*B1M_raw_x(i))-(1-lam)/2*(usim_k1(i)+2*usim_k2(i)+usim_k3(i)))-usim_k1(i);
    usim_k3(i+1) = usim_k2(i);
    usim_k2(i+1) = usim_k1(i);
    usim_k1(i+1) = usim(i);
    B1T_raw_x_d3(i+1) = B1T_raw_x_d2(i);
    B1T_raw_y_d3(i+1) = B1T_raw_y_d2(i);
    B1T_raw_z_d3(i+1) = B1T_raw_z_d2(i);
    B1T_raw_x_d2(i+1) = B1T_raw_x_d1(i);
    B1T_raw_y_d2(i+1) = B1T_raw_y_d1(i);
    B1T_raw_z_d2(i+1) = B1T_raw_z_d1(i);
    B1T_raw_x_d1(i+1) = B1T_raw_x(i);
    B1T_raw_y_d1(i+1) = B1T_raw_y(i);
    B1T_raw_z_d1(i+1) = B1T_raw_z(i);
end


% 
Delta_x = B1T_raw_x_d3(1:length(B1M_raw_x))'-B1M_raw_x;
Delta_y = B1T_raw_y_d3(1:length(B1M_raw_x))'-B1M_raw_y;
Delta_z = B1T_raw_z_d3(1:length(B1M_raw_x))'-B1M_raw_z;
% Delta_x = B1T_raw_x-B1M_raw_x;
% Delta_y = B1T_raw_y-B1M_raw_y;
% Delta_z = B1T_raw_z-B1M_raw_z;
for i = 1:length(Delta_x)
     if i<=FFF/2
         mean_running_x(i) = sum(Delta_x(i:i+FFF-1))/FFF;
         mean_running_y(i) = sum(Delta_y(i:i+FFF-1))/FFF;
         mean_running_z(i) = sum(Delta_z(i:i+FFF-1))/FFF;

     elseif i>=length(Delta_x)-25
         mean_running_x(i) = sum(Delta_x(i-FFF-1:i))/FFF;
         mean_running_y(i) = sum(Delta_y(i-FFF-1:i))/FFF;
         mean_running_z(i) = sum(Delta_z(i-FFF-1:i))/FFF;
     else
         mean_running_x(i) = sum(Delta_x(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_y(i) = sum(Delta_y(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_z(i) = sum(Delta_z(i-FFF/2:i+FFF/2))/(FFF+1);
     end
end
Delta_brown_x = Delta_x-mean_running_x';
Delta_brown_y = Delta_y-mean_running_y';
Delta_brown_z = Delta_z-mean_running_z';

% for i = 1:length(B1M_raw_x)
%     if i==1
%         mean_running_deltax(i) = 0;
%         mean_running_deltay(i) = 0;
%         mean_running_deltaz(i) = 0;
%     else
%         mean_running_deltax(i) = 1600*(mean_running_x(i)-mean_running_x(i-1));
%         mean_running_deltay(i) = 1600*(mean_running_y(i)-mean_running_y(i-1));
%         mean_running_deltaz(i) = 1600*(mean_running_z(i)-mean_running_z(i-1));
%         
%     end
% end

FxdragOm_unfilter = gx.*deltax;
FydragOm_unfilter = gy.*deltay;
FzdragOm_unfilter = gz.*deltaz;
FxdragOm = gx.*mean_running_deltax;
FydragOm = gy.*mean_running_deltay;
FzdragOm = gz.*mean_running_deltaz;

Fxrandom = FxdragOm-FxdragOm_unfilter;
Fyrandom = FydragOm-FydragOm_unfilter;
Fzrandom = FzdragOm-FzdragOm_unfilter;


for i = 1:length(FxdragOm)
%      if i<=25
%          mean_running_Fx(i) = sum(FxdragOm(i:i+49))/50;
%          mean_running_Fy(i) = sum(FydragOm(i:i+49))/50;
%          mean_running_Fz(i) = sum(FzdragOm(i:i+49))/50;
%          mean_running_ux(i) = sum(Fx_Om(i:i+49))/50;
%          mean_running_uy(i) = sum(Fy_Om(i:i+49))/50;
%          mean_running_uz(i) = sum(Fz_Om(i:i+49))/50;
% 
%      elseif i>=length(FydragOm)-25
%          mean_running_Fx(i) = sum(FxdragOm(i-49:i))/50;
%          mean_running_Fy(i) = sum(FydragOm(i-49:i))/50;
%          mean_running_Fz(i) = sum(FzdragOm(i-49:i))/50;
%          mean_running_ux(i) = sum(Fx_Om(i-49:i))/50;
%          mean_running_uy(i) = sum(Fy_Om(i-49:i))/50;
%          mean_running_uz(i) = sum(Fz_Om(i-49:i))/50;
%      else
%          mean_running_Fx(i) = sum(FxdragOm(i-25:i+25))/51;
%          mean_running_Fy(i) = sum(FydragOm(i-25:i+25))/51;
%          mean_running_Fz(i) = sum(FzdragOm(i-25:i+25))/51;
%          mean_running_ux(i) = sum(Fx_Om(i-25:i+25))/51;
%          mean_running_uy(i) = sum(Fy_Om(i-25:i+25))/51;
%          mean_running_uz(i) = sum(Fz_Om(i-25:i+25))/51;
%      end
     
     if i<=FFF/2
         mean_running_Fx(i) = sum(FxdragOm(i:i+FFF-1))/FFF;
         mean_running_Fy(i) = sum(FydragOm(i:i+FFF-1))/FFF;
         mean_running_Fz(i) = sum(FzdragOm(i:i+FFF-1))/FFF;
         mean_running_ux(i) = sum(Fx_Om(i:i+FFF-1))/FFF;
         mean_running_uy(i) = sum(Fy_Om(i:i+FFF-1))/FFF;
         mean_running_uz(i) = sum(Fz_Om(i:i+FFF-1))/FFF;
         mean_running_ux_current_Om(i) = sum(Fx_current_Om(i:i+FFF-1))/FFF;
         mean_running_uy_current_Om(i) = sum(Fy_current_Om(i:i+FFF-1))/FFF;
         mean_running_uz_current_Om(i) = sum(Fz_current_Om(i:i+FFF-1))/FFF;
     elseif i>=length(FydragOm)-FFF/2
         mean_running_Fx(i) = sum(FxdragOm(i-FFF-1:i))/FFF;
         mean_running_Fy(i) = sum(FydragOm(i-FFF-1:i))/FFF;
         mean_running_Fz(i) = sum(FzdragOm(i-FFF-1:i))/FFF;
         mean_running_ux(i) = sum(Fx_Om(i-FFF-1:i))/FFF;
         mean_running_uy(i) = sum(Fy_Om(i-FFF-1:i))/FFF;
         mean_running_uz(i) = sum(Fz_Om(i-FFF-1:i))/FFF;
         mean_running_ux_current_Om(i) = sum(Fx_current_Om(i-FFF-1:i))/FFF;
         mean_running_uy_current_Om(i) = sum(Fy_current_Om(i-FFF-1:i))/FFF;
         mean_running_uz_current_Om(i) = sum(Fz_current_Om(i-FFF-1:i))/FFF;
     else
         mean_running_Fx(i) = sum(FxdragOm(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_Fy(i) = sum(FydragOm(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_Fz(i) = sum(FzdragOm(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_ux(i) = sum(Fx_Om(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_uy(i) = sum(Fy_Om(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_uz(i) = sum(Fz_Om(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_ux_current_Om(i) = sum(Fx_current_Om(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_uy_current_Om(i) = sum(Fy_current_Om(i-FFF/2:i+FFF/2))/(FFF+1);
         mean_running_uz_current_Om(i) = sum(Fz_current_Om(i-FFF/2:i+FFF/2))/(FFF+1);
     end
end
% for i = 1:length(FxdragOm)
%     FdragOm = [mean_running_Fx(i);mean_running_Fy(i);mean_running_Fz(i)];
%     FdragOa = id_m2a*FdragOm;
%     FxdragOa(i) = FdragOa(1);
%     FydragOa(i) = FdragOa(2);
%     FzdragOa(i) = FdragOa(3);
% end

for i = 1:length(FxdragOm)
    FdragOm = [FxdragOm(i);FydragOm(i);FzdragOm(i)];
    FdragOa = id_m2a*FdragOm;
    FxdragOa(i) = FdragOa(1);
    FydragOa(i) = FdragOa(2);
    FzdragOa(i) = FdragOa(3);
    if i==1
        HsVd_ch1_d1(i) = 0;
        HsVd_ch2_d1(i) = 0;
        HsVd_ch3_d1(i) = 0;
        HsVd_ch4_d1(i) = 0;
        HsVd_ch5_d1(i) = 0;
        HsVd_ch6_d1(i) = 0;
    else
        HsVd_ch1_d1(i) = HsVd_ch1(i-1);
        HsVd_ch2_d1(i) = HsVd_ch2(i-1);
        HsVd_ch3_d1(i) = HsVd_ch3(i-1);
        HsVd_ch4_d1(i) = HsVd_ch4(i-1);
        HsVd_ch5_d1(i) = HsVd_ch5(i-1);
        HsVd_ch6_d1(i) = HsVd_ch6(i-1);
    end
    
end


% figure(1)
% subplot(3,1,1);
% plot(t(aaa:bbb), FD_x_oa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fx_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fx(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(t(aaa:bbb), FD_y_oa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fy_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fy(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(t(aaa:bbb), FD_z_oa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fz_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fz(pN)');
% xlabel('Time(s)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% 
% figure(2)
% subplot(3,1,1);
% plot(FD_x_oa(aaa:bbb),Fx_Oa(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% xlabel('Fdx(pN)');
% ylabel('Fx(model)(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(FD_y_oa(aaa:bbb),Fy_Oa(aaa:bbb) ,'b', 'linewidth', 1.5);%
% grid on;axis tight;
% xlabel('Fdy(pN)');
% ylabel('Fy(model)(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(FD_z_oa(aaa:bbb),Fz_Oa(aaa:bbb) ,'b', 'linewidth', 1.5);%
% grid on;axis tight;
% xlabel('Fdz(pN)');
% ylabel('Fz(model)(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% 
% figure(3)
% subplot(3,1,1);
% plot(t(aaa:bbb), FxdragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fx_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fx(pN)');
% legend('Drag force','Magnetic force')
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(t(aaa:bbb), FydragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fy_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fy(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(t(aaa:bbb), FzdragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), Fz_Oa(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Fz(pN)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% 
% figure(4)
% subplot(3,1,1);
% plot(Fx_Oa(aaa:bbb), FxdragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Fdragx(pN)');
% xlabel('Fmagx(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(Fy_Oa(aaa:bbb), FydragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Fdragy(pN)');
% xlabel('Fmagy(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(Fz_Oa(aaa:bbb), FzdragOa(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Fdragz(pN)');
% xlabel('Fmagz(pN)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)




figure(4)
% h=figure('position',[100 100 800 600]);
subplot(3,1,1);
plot(t(aaa:bbb), B1T_raw_x(aaa:bbb)-B1M_raw_x(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error X (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), B1T_raw_y(aaa:bbb)-(B1M_raw_y(aaa:bbb)),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error Y (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), B1T_raw_z(aaa:bbb)-B1M_raw_z(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error Z (um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(5)
% h=figure('position',[100 100 800 600]);
subplot(3,1,1);
plot(t(aaa:bbb), B1M_raw_x(aaa:bbb),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_x(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos X (um)');
legend('Measurement','Target')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), (B1M_raw_y(aaa:bbb)),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_y(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos Y (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), B1M_raw_z(aaa:bbb),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_z(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos Z (um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


% figure(4)
% % h=figure('position',[100 100 800 600]);
% subplot(3,1,1);
% plot(t(aaa:bbb), B1T_raw_x(aaa:bbb)-B1M_raw_x(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Error X (um)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(t(aaa:bbb), B1T_raw_y(aaa:bbb)-1.1*(B1M_raw_y(aaa:bbb)+0.05),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Error Y (um)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(t(aaa:bbb), B1T_raw_z(aaa:bbb)-1.2*(B1M_raw_z(aaa+10:bbb+10)-mean(B1M_raw_z(aaa+10:bbb+10)))-mean(B1M_raw_z(aaa+10:bbb+10)),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Error Z (um)');
% xlabel('Time(s)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)



% figure(5)
% % h=figure('position',[100 100 800 600]);
% subplot(3,1,1);
% plot(t(aaa:bbb), B1M_raw_x(aaa:bbb),'b', 'linewidth', 1);%
% hold on
% plot(t(aaa:bbb), B1T_raw_x(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Pos X (um)');
% legend('Measurement','Target')
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,2);
% plot(t(aaa:bbb), 1.1*(B1M_raw_y(aaa:bbb)+0.05),'b', 'linewidth', 1);%
% hold on
% plot(t(aaa:bbb), B1T_raw_y(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Pos Y (um)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,1,3);
% plot(t(aaa:bbb), 1.2*(B1M_raw_z(aaa+10:bbb+10)-mean(B1M_raw_z(aaa+10:bbb+10)))+mean(B1M_raw_z(aaa+10:bbb+10)),'b', 'linewidth', 1);%
% hold on
% plot(t(aaa:bbb), B1T_raw_z(aaa:bbb),'r', 'linewidth', 1);%
% grid on;axis tight;
% ylabel('Pos Z (um)');
% xlabel('Time(s)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)



figure(6)
subplot(3,1,1);
plot(t(aaa:bbb), FD_x_om(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fx_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fx(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), FD_y_om(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fy_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fy(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), FD_z_om(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fz_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fz(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(7)
subplot(3,1,1);
plot(FD_x_om(aaa:bbb),Fx_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
xlabel('Fdx(pN)');
ylabel('Fx(model)(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(FD_y_om(aaa:bbb),Fy_Om(aaa:bbb) ,'b', 'linewidth', 1.5);%
grid on;axis tight;
xlabel('Fdy(pN)');
ylabel('Fy(model)(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(FD_z_om(aaa:bbb),Fz_Om(aaa:bbb) ,'b', 'linewidth', 1.5);%
grid on;axis tight;
xlabel('Fdz(pN)');
ylabel('Fz(model)(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(8)
subplot(3,1,1);
plot(t(aaa:bbb), FxdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fx_Om(aaa:bbb)-mean(Fx_Om(ccc:ddd)),'r', 'linewidth', 1);%
% plot(t(aaa:bbb), Fx_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fx(pN)');
legend('Drag force','Magnetic force')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), FydragOm(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fy_Om(aaa:bbb)-mean(Fy_Om(ccc:ddd)),'r', 'linewidth', 1);%
% plot(t(aaa:bbb), Fy_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fy(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), FzdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fz_Om(aaa:bbb)-mean(Fz_Om(ccc:ddd)),'r', 'linewidth', 1);%
% plot(t(aaa:bbb), Fz_Om(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fz(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(9)
subplot(3,1,1);
plot(Fx_Om(aaa:bbb)-mean(Fx_Om(ccc:ddd)), FxdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragx(pN)');
xlabel('Fmagx(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(Fy_Om(aaa:bbb)-mean(Fy_Om(ccc:ddd)), FydragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragy(pN)');
xlabel('Fmagy(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(Fz_Om(aaa:bbb)-mean(Fz_Om(ccc:ddd)), FzdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragz(pN)');
xlabel('Fmagz(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(10)
subplot(3,1,1);
plot(t(aaa:bbb), mean_running_ux(aaa:bbb)-mean(mean_running_ux(ccc:ddd)),'r', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), 0.55*FxdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fx(pN)');
legend('Magnetic force','Drag force')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), mean_running_uy(aaa:bbb)-mean(mean_running_uy(ccc:ddd)),'r', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), 1.1*FydragOm(aaa:bbb),'b', 'linewidth', 1.5);% 0.8
grid on;axis tight;
ylabel('Fy(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), (mean_running_uz(aaa:bbb)-mean(mean_running_uz(ccc:ddd))),'r', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), 1.2*FzdragOm(aaa:bbb),'b', 'linewidth', 1.5);% 
grid on;axis tight;
ylabel('Fz(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(11)
subplot(3,1,1);
plot(mean_running_ux(aaa:bbb)-mean(mean_running_ux(ccc:ddd)), 0.8*FxdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragx(pN)');
xlabel('Fmagx(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(mean_running_uy(aaa:bbb)-mean(mean_running_uy(ccc:ddd)), FydragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragy(pN)');
xlabel('Fmagy(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot((mean_running_uz(aaa:bbb)-mean(mean_running_uz(ccc:ddd))), 1.2*FzdragOm(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragz(pN)');
xlabel('Fmagz(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

%     x = mean_running_ux(aaa:bbb)-mean(mean_running_ux(ccc:ddd)); 
%     y1 = mean_running_Fx(aaa:bbb); 
%         x1 = mean_running_uy(aaa:bbb)-mean(mean_running_uy(ccc:ddd))-0.2; 
%     y2 = mean_running_Fy(aaa:bbb); 
%         x2 = mean_running_uz(aaa:bbb)-mean(mean_running_uz(ccc:ddd)); 
%     y3 = mean_running_Fz(aaa:bbb); 
%     P = polyfit(x,y1,1);
%     P1 = polyfit(x1,y2,1);
%     P2 = polyfit(x2,y3,1);
%     figure(18)
%     yfit = P(1)*x+P(2);
%     hold on;
%     plot(x,yfit,'r-.');


figure(12)
subplot(3,1,1);
plot(t(aaa:bbb), FxdragOm_unfilter(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fx_Om(aaa:bbb)-mean(Fx_Om(ccc:ddd)),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fdragx(pN)');
xlabel('Time(sec)');
legend('Drag force','Magnetic force')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), FydragOm_unfilter(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fy_Om(aaa:bbb)-mean(Fy_Om(ccc:ddd)),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fdragy(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), FzdragOm_unfilter(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), Fz_Om(aaa:bbb)-mean(Fz_Om(ccc:ddd)),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Fdragz(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(13)
subplot(3,1,1);
plot(t(aaa:bbb), Fxrandom(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fxrandom(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), Fyrandom(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fyrandom(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), Fzrandom(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fzrandom(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(15)
subplot(3,2,1);
plot(DA_ch1(aaa:bbb), -HsVm_ch1(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh1(V)');
xlabel('Current 1(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,2);
plot(DA_ch2(aaa:bbb), HsVm_ch2(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh2(V)');
xlabel('Current 2(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,3);
plot(DA_ch3(aaa:bbb), -HsVm_ch3(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh3(V)');
xlabel('Current 3(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,4);
plot(DA_ch4(aaa:bbb), HsVm_ch4(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh4(V)');
xlabel('Current 4(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,5);
plot(DA_ch5(aaa:bbb), HsVm_ch5(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh5(V)');
xlabel('Current 5(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,6);
plot(DA_ch6(aaa:bbb), -HsVm_ch6(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Vh6(V)');
xlabel('Current 6(A)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)



% figure(17)
% subplot(3,2,1);
% plot(t(aaa:bbb), HsVm_ch1(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch1_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh1(V)');
% xlabel('Time(sec)');
% legend('Vh','Vd')
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,2);
% plot(t(aaa:bbb), HsVm_ch2(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch2_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh2(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,3);
% plot(t(aaa:bbb), HsVm_ch3(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch3_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh3(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,4);
% plot(t(aaa:bbb), HsVm_ch4(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch4_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh4(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,5);
% plot(t(aaa:bbb), HsVm_ch5(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch5_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh5(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,6);
% plot(t(aaa:bbb), HsVm_ch6(aaa:bbb),'b', 'linewidth', 1.5);%
% hold on
% plot(t(aaa:bbb), HsVd_ch6_d1(aaa:bbb),'r', 'linewidth', 1.2);%
% grid on;axis tight;
% ylabel('Vh6(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(17)
subplot(3,2,1);
plot(t(aaa:bbb), HsVm_ch1(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch1(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh1(V)');
xlabel('Time(sec)');
legend('Vh','Vd')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,2);
plot(t(aaa:bbb), HsVm_ch2(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch2(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh2(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,3);
plot(t(aaa:bbb), HsVm_ch3(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch3(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh3(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,4);
plot(t(aaa:bbb), HsVm_ch4(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch4(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh4(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,5);
plot(t(aaa:bbb), HsVm_ch5(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch5(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh5(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,6);
plot(t(aaa:bbb), HsVm_ch6(aaa:bbb),'b', 'linewidth', 1.5);%
hold on
plot(t(aaa:bbb), HsVd_ch6(aaa:bbb),'r', 'linewidth', 1.2);%
grid on;axis tight;
ylabel('Vh6(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% figure(19)
% subplot(3,2,1);
% plot(t(aaa:bbb), HsVd_ch1_d1(aaa:bbb)'-HsVm_ch1(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V1(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,2);
% plot(t(aaa:bbb), HsVd_ch2_d1(aaa:bbb)'-HsVm_ch2(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V2(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,3);
% plot(t(aaa:bbb), HsVd_ch3_d1(aaa:bbb)'-HsVm_ch3(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V3(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,4);
% plot(t(aaa:bbb), HsVd_ch4_d1(aaa:bbb)'-HsVm_ch4(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V4(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,5);
% plot(t(aaa:bbb), HsVd_ch5_d1(aaa:bbb)'-HsVm_ch5(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V5(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
% subplot(3,2,6);
% plot(t(aaa:bbb), HsVd_ch6_d1(aaa:bbb)'-HsVm_ch6(aaa:bbb),'b', 'linewidth', 1.5);%
% grid on;axis tight;
% ylabel('Delta V6(V)');
% xlabel('Time(sec)');
% set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(19)
subplot(3,2,1);
plot(t(aaa:bbb), HsVd_ch1(aaa:bbb)-HsVm_ch1(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V1(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,2);
plot(t(aaa:bbb), HsVd_ch2(aaa:bbb)-HsVm_ch2(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V2(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,3);
plot(t(aaa:bbb), HsVd_ch3(aaa:bbb)-HsVm_ch3(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V3(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,4);
plot(t(aaa:bbb), HsVd_ch4(aaa:bbb)-HsVm_ch4(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V4(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,5);
plot(t(aaa:bbb), HsVd_ch5(aaa:bbb)-HsVm_ch5(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V5(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,2,6);
plot(t(aaa:bbb), HsVd_ch6(aaa:bbb)-HsVm_ch6(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Delta V6(V)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
%% 


figure(3)
% h=figure('position',[100 100 800 600]);
subplot(3,1,1);
plot(t(aaa:bbb), B1M_raw_x(aaa:bbb),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_x_d3(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos X (um)');
legend('Measurement','Target')
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), (B1M_raw_y(aaa:bbb)),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_y_d3(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos Y (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), B1M_raw_z(aaa:bbb),'b', 'linewidth', 1);%
hold on
plot(t(aaa:bbb), B1T_raw_z_d3(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Pos Z (um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(14)
% h=figure('position',[100 100 800 600]);
subplot(3,1,1);
plot(t(aaa:bbb), B1T_raw_x_d3(aaa:bbb)'-B1M_raw_x(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error X (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), B1T_raw_y_d3(aaa:bbb)'-B1M_raw_y(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error Y (um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), B1T_raw_z_d3(aaa:bbb)'-B1M_raw_z(aaa:bbb),'r', 'linewidth', 1);%
grid on;axis tight;
ylabel('Error Z (um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(16)
subplot(3,1,1);
plot(Fx_Om(aaa:bbb), Fx_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fx current(pN)');
xlabel('Fx voltage(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(Fy_Om(aaa:bbb), Fy_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fy current(pN)');
xlabel('Fy voltage(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(Fz_Om(aaa:bbb), Fz_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fz current(pN)');
xlabel('Fz voltage(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(18)
subplot(3,1,1);
plot(mean_running_ux_current_Om(aaa:bbb), mean_running_Fx(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragx(pN)');
xlabel('Fx current(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(mean_running_uy_current_Om(aaa:bbb), mean_running_Fy(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragy(pN)');
xlabel('Fy current(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(mean_running_uz_current_Om(aaa:bbb), mean_running_Fz(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fdragz(pN)');
xlabel('Fz current(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(20)
subplot(3,1,1);
plot(t(aaa:bbb), Fx_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fx current(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), Fy_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fy current(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), Fz_current_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fz current(pN)');
xlabel('Time(sec)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(21)
subplot(3,1,1);
plot(FD_x_om_new(aaa:bbb), Fx_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fxmag voltage(pN)');
xlabel('Fd x(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(FD_y_om_new(aaa:bbb), Fy_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fymag voltage(pN)');
xlabel('Fd y(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(FD_z_om_new(aaa:bbb), Fz_Om(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fzmag voltage(pN)');
xlabel('Fd z(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

%     x = FD_x_om_new(aaa:bbb)-mean(FD_x_om_new(ccc:ddd)); 
%     y1 = Fx_Om(aaa:bbb); 
%         x1 = FD_y_om_new(aaa:bbb)-mean(FD_y_om_new(ccc:ddd))-0.2; 
%     y2 = Fy_Om(aaa:bbb); 
%         x2 = FD_z_om_new(aaa:bbb)-mean(FD_z_om_new(ccc:ddd)); 
%     y3 = Fz_Om(aaa:bbb); 
%         x = FD_x_om(aaa:bbb)-mean(FD_x_om(ccc:ddd)); 
%     y1 = Fx_Om(aaa:bbb); 
%         x1 = FD_y_om(aaa:bbb)-mean(FD_y_om(ccc:ddd)); 
%     y2 = Fy_Om(aaa:bbb); 
%         x2 = FD_z_om(aaa:bbb)-mean(FD_z_om(ccc:ddd)); 
%     y3 = Fz_Om(aaa:bbb); 
            x = FD_x_om(aaa:bbb); 
    y1 = Fx_Om(aaa:bbb); 
        x1 = FD_y_om(aaa:bbb); 
    y2 = Fy_Om(aaa:bbb); 
        x2 = FD_z_om(aaa:bbb); 
    y3 = Fz_Om(aaa:bbb); 
    P = polyfit(x,y1,1);
    P1 = polyfit(x1,y2,1);
    P2 = polyfit(x2,y3,1);

figure(22)
subplot(3,1,1);
plot(FD_x_om(aaa:bbb)-mean(FD_x_om(aaa:bbb)), Fx_Om(aaa:bbb)-mean(Fx_Om(aaa:bbb)),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fxmag voltage(pN)');
xlabel('Fd x(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(FD_y_om(aaa:bbb)-mean(FD_y_om(aaa:bbb)), Fy_Om(aaa:bbb)-mean(Fy_Om(aaa:bbb)),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fymag voltage(pN)');
xlabel('Fd y(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(FD_z_om(aaa:bbb)-mean(FD_z_om(aaa:bbb)), Fz_Om(aaa:bbb)-mean(Fz_Om(aaa:bbb)),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Fzmag voltage(pN)');
xlabel('Fd z(pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(23)
subplot(3,1,1);
histogram(mean_running_ux(aaa:bbb)-mean(mean_running_ux(ccc:ddd))-0.55*FxdragOm(aaa:bbb))
grid on;axis tight;
xlabel('Error Fx (pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
histogram(mean_running_uy(aaa:bbb)-mean(mean_running_uy(ccc:ddd))-0.8*FydragOm(aaa:bbb))
grid on;axis tight;
xlabel('Error Fy (pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
histogram(mean_running_uz(aaa:bbb)-mean(mean_running_uz(ccc:ddd))-1.2*FzdragOm(aaa:bbb))
grid on;axis tight;
xlabel('Error Fz (pN)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(25)
subplot(2,1,1);
plot(t(aaa:bbb), gx(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Gamma xy(pN sec/um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(2,1,2);
plot(t(aaa:bbb), gz(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Gamma z(pN sec/um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(26)
subplot(3,1,1);
plot(t(aaa:bbb), Delta_brown_x(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Random x(um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), Delta_brown_y(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Random y(um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), Delta_brown_z(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Random z(um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(27)
subplot(3,1,1);
plot(t(aaa:bbb), FD_x_om_new(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Ux(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), FD_y_om_new(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Uy(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), FD_z_om_new(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Uz(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(28)
subplot(3,1,1);
plot(t(aaa:bbb), Dx_FPGA(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Dx(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
plot(t(aaa:bbb), Dy_FPGA(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Dy(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
plot(t(aaa:bbb), Dz_FPGA(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Dz(pN)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)


figure(29)
subplot(3,1,1);
histogram(Delta_brown_x(aaa:bbb))
grid on;axis tight;
xlabel('Random motion x(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
histogram(Delta_brown_y(aaa:bbb))
grid on;axis tight;
xlabel('Random motion y(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
histogram(Delta_brown_z(aaa:bbb))
grid on;axis tight;
xlabel('Random motion z(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(30)
subplot(3,1,1);
histogram(Fxrandom(aaa:bbb))
grid on;axis tight;
xlabel('Random Force x(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,2);
histogram(Fyrandom(aaa:bbb))
grid on;axis tight;
xlabel('Random Force y(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(3,1,3);
histogram(Fzrandom(aaa:bbb))
grid on;axis tight;
xlabel('Random Force z(um)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)

figure(31)
subplot(2,1,1);
plot(t(aaa:bbb), gxy_FPGA(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Gamma xy(pN sec/um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
subplot(2,1,2);
plot(t(aaa:bbb), gz_FPGA(aaa:bbb),'b', 'linewidth', 1.5);%
grid on;axis tight;
ylabel('Gamma z(pN sec/um)');
xlabel('Time(s)');
set(gca,'FontSize',12,'FontWeight','bold','linewidth',1.5)
