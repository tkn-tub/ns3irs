function [r_a_c,a_a_c,r_c_b,a_c_b] = calcangle(a,b,c,n)

ac = c - a;
cb = b - c;

a_a_c = rad2deg(atan2(norm(cross(ac,n)),dot(ac,n)));
a_c_b = rad2deg(atan2(norm(cross(cb, n)),dot(cb, n)));

r_a_c = norm(a-c);
r_c_b = norm(b-c);

end