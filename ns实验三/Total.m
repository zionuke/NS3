data1 = csvread('6nodePF.csv'); % 如果不在工作目录，那就写完整的路径
x=data1(:,1);%x轴上的数据
y1=data1(:,10); %a数据y值
data2 = csvread('6nodeRR.csv'); % 如果不在工作目录，那就写完整的路径
y2=data2(:,10); %a数据y值
data3 = csvread('6nodeMR.csv'); % 如果不在工作目录，那就写完整的路径
y3=data3(:,10); %a数据y值
h=plot(x,y1,'-*b',x,y2,'-ok',x,y3,'-+r');  %线性，颜色，标记
axis([400,1200,0,500])  %确定x轴与y轴框图大小
set(gca,'XTick',[400:200:1200]) 
set(gca,'YTick',[0:100:500]) 
set(gcf,'DefaultAxesFontName','times');
set(gcf,'DefaultAxesFontSize',14);
legend([h(1),h(2),h(3)],'PF','RR','MR');
name='SumThroughput';
title(name);   %右上角标注
xlabel('distance/m')  %x轴坐标描述
ylabel('Throughput/Mbps') %y轴坐标描述
print(gcf,'-dpng',name);

