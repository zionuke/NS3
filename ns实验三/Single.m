data = csvread('CQI4_1_1800.csv'); % 如果不在工作目录，那就写完整的路径
x=data(:,1);%x轴上的数据
y1=data(:,2); %a数据y值
y2=data(:,3); %a数据y值
y3=data(:,4); %a数据y值
y4=data(:,5); %a数据y值
plot(x,y1,'-*r',x,y2,'-og',x,y3,'-+b',x,y4,'-xc'); %线性，颜色，标记
axis([0,30,0,10])  %确定x轴与y轴框图大小
set(gca,'XTick',[0:0.5:30]) 
set(gca,'YTick',[0:1:10]) 
set(gcf,'DefaultAxesFontName','times');
set(gcf,'DefaultAxesFontSize',14);
legend('UE1','UE2','UE3','UE4');
name='CQI-1800m-PF';
title(name);   %右上角标注
xlabel('time/s')  %x轴坐标描述
ylabel('CQI') %y轴坐标描述
print(gcf,'-dpng',name);
