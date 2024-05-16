function Figurebar2ax1(Figure_)
%FIGURE bar 2ax1: 2 bars, 1 axes

fig=figure;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Lines on the Left
bar(Figure_.bin1,Figure_.bar1,Figure_.BARWIDTH1,'FaceColor',Figure_.color1);
hold on
bar(Figure_.bin2,Figure_.bar2,Figure_.BARWIDTH2,'FaceColor',Figure_.color2);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
hold off
% Vertical Axis
legy1=ylabel(Figure_.legy1,'Color',Figure_.legy1color);
% Horizontal Axis
legx=xlabel(Figure_.xlabel);
% Legend
leg = legend(Figure_.leg1,Figure_.leg2,'Location',Figure_.legendposition);
% Title
legtitle = title(Figure_.legtitle);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Settings of the Figure
set(leg,'Interpreter','latex','FontSize',12);
set(legx,'Interpreter','latex','FontSize',13);
set(legy1,'Interpreter','latex','FontSize',13);
set(legtitle,'Interpreter','latex','FontSize',13);
%ax = gca;
%ax.YAxis(1).Direction = 'reverse';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
saveas(gcf,strcat(Figure_.outputdir,Figure_.filename));
end