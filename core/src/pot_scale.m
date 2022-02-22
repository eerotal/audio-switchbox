clear all
clf

x1 = round([0:10:100]'.*(255/100));
y1 = [0 1 2.5 7.5 13 17.5 22 36.5 67 95 100]'.*(255/100);

x2 = [0:1:255]';
y2 = interp1(x1, y1, x2, 'cubic');

hold on
plot(x1, y1)
plot(x2, y2)

f = round(y2)