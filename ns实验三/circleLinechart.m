data = csvread('circleMR.csv'); % 如果不在工作目录，那就写完整的路径
x=data(:,1);%x轴上的数据
y=data(:,6); %a数据y值
plot(x,y,'-*b'); %线性，颜色，标记
axis([200,1800,0,400])  %确定x轴与y轴框图大小
set(gca,'XTick',[200:400:1800]) 
set(gca,'YTick',[0:100:400]) 
set(gcf,'DefaultAxesFontName','times');
set(gcf,'DefaultAxesFontSize',14);
name='sumThroughput';
title(name);   %右上角标注
xlabel('半径/米')  %x轴坐标描述
ylabel('吞吐量/Mbps') %y轴坐标描述
print(gcf,'-dpng',name);