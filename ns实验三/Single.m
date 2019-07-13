data = csvread('CentripetalPF.csv'); % 如果不在工作目录，那就写完整的路径
x=data(:,1);%x轴上的数据
y1=data(:,2); %a数据y值
y2=data(:,3); %a数据y值
y3=data(:,4); %a数据y值
y4=data(:,5); %a数据y值
plot(x,y1,'-*b',x,y2,'-ok',x,y3,'-+r',x,y4,'-xg'); %线性，颜色，标记
axis([200,1800,0,100])  %确定x轴与y轴框图大小
set(gca,'XTick',[200:400:1800]) 
set(gca,'YTick',[0:20:100]) 
set(gcf,'DefaultAxesFontName','times');
set(gcf,'DefaultAxesFontSize',14);
legend('UE1','UE2','UE3','UE4');
name='CentripetalPF-UEthroughput';
title(name);   %右上角标注
xlabel('半径/米')  %x轴坐标描述
ylabel('吞吐量/Mbps') %y轴坐标描述
print(gcf,'-dpng',name);