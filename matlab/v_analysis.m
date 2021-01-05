clf
clear

% Load data from CSV files
data_1 = load("data_1.csv");
dt = data_1(1, 1); % Sampling period
t = data_1(:, 1); % Time
v_x = data_1(:, 2); % X velocities
v_y = data_1(:, 3); % Y velocities
alt = data_1(:, 4); % Altitudes

N = length(data_1); % Number of samples
F_s = 1 / dt; % Signal sampling frequency

raw_data = load("raw_data.csv");
raw_v = raw_data(1:N, 2); % Raw velocities
raw_alt = raw_data(1:N, 3); % Raw altitudes

% Filter velocity signals
break_freq_1 = 0;
break_freq_2 = 1;

[v_x_filtered, v_x_delay] = pm_lp_filter(v_x, F_s, [break_freq_1 break_freq_2], 0.001);
[v_y_filtered, v_y_delay] = pm_lp_filter(v_y, F_s, [break_freq_1 break_freq_2], 0.001);

% Phase shift by delay introduced by filter
t = t(1:end - v_x_delay);
v_x = v_x(1:end - v_x_delay);
v_y = v_y(1:end - v_x_delay);
alt = alt(1:end - v_x_delay);
raw_v = raw_v(1:end - v_x_delay);
raw_alt = raw_alt(1:end - v_x_delay);

% Set up for plotting
v = sqrt(v_x.^2 + v_y.^2);
v_filtered = sqrt(v_x_filtered.^2 + v_y_filtered.^2);

v_y_filtered_integral = zeros(length(v_y_filtered), 1);
v_y_filtered_integral(1, 1) = v_y_filtered(1) * dt;
for i = 2:length(v_y_filtered)
    delta_t = t(i) - t(i - 1);
    dy = v_y_filtered(i) * delta_t;
    v_y_filtered_integral(i, 1) = v_y_filtered_integral(i - 1, 1) + dy;
end
v_y_filtered_integral = v_y_filtered_integral / 1000;

% Write output to file
writematrix([t v_x_filtered v_y_filtered], "data_2.csv");

% Plotting
% X velocity
figure(1);

subplot(2, 2, 1);
plot_fft(v_x, F_s);
title("X Velocity FFT");
xlabel("Frequency (Hz)");
ylabel("FFT Magnitude");
axis([0 16 0 1e5]);
grid();

subplot(2, 2, 2);
plot(t, v_x);
title("X Velocity");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
grid();

subplot(2, 2, 3);
plot_fft(v_x_filtered, F_s);
title("Filtered X Velocity FFT");
xlabel("Frequency (Hz)");
ylabel("FFT Magnitude");
axis([0 16 0 1e5]);
grid();

subplot(2, 2, 4);
plot(t, v_x_filtered);
title("Filtered X Velocity");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
grid();

% Y velocity
figure(2);

subplot(2, 2, 1);
plot_fft(v_y, F_s);
title("Y Velocity FFT");
xlabel("Frequency (Hz)");
ylabel("FFT Magnitude");
axis([0 16 0 1e5]);
grid();

subplot(2, 2, 2);
plot(t, v_y);
title("Y Velocity");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
grid();

subplot(2, 2, 3);
plot_fft(v_y_filtered, F_s);
title("Filtered Y Velocity FFT");
xlabel("Frequency (Hz)");
ylabel("FFT Magnitude");
axis([0 16 0 1e5]);
grid();

subplot(2, 2, 4);
plot(t, v_y_filtered);
title("Filtered Y Velocity");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
grid();

% Velocity errors
figure(3);

subplot(2, 1, 1);
plot(t, v, t, v_filtered);
title("Velocity magnitude vs. Filtered velocity magnitude");
xlabel("Time (s)");
ylabel("Velocity (m/s)");
legend("Velocity", "Filtered velocity");
grid();

subplot(2, 1, 2);
plot(t, v_filtered - v, t, v_filtered - raw_v);
title("Velocity magnitude error");
xlabel("Time (s)");
ylabel("Error (m/s)");
legend("Processed Velocity Error", "Raw Velocity Error");
grid();

% Altitude errors
figure(4);

subplot(2, 1, 1);
plot(t, alt, t, v_y_filtered_integral);
title("Altitude vs. Filtered Y velocity integral");
xlabel("Time (s)");
ylabel("Altitude (km)");
legend("Altitude", "Filtered velocity integral");
grid();

subplot(2, 1, 2);
plot(t, v_y_filtered_integral - alt, t, v_y_filtered_integral - raw_alt);
title("Altitude error");
xlabel("Time (s)");
ylabel("Error (km)");
legend("Processed Altitude Error", "Raw Altitude Error");
grid();

% Functions
% Plot the FFT of a signal at the given sampling rate
function plot_fft(signal, F_s)
    N_signal = length(signal); % # of samples in signal
    if (mod(N_signal, 2) == 0) % # of FFT points used
        N_fft = N_signal / 2;
    else
        N_fft = (N_signal + 1) / 2;
    end
    
    freq_axis = linspace(0, F_s / 2, N_fft); % Take one-sided frequencies

    signal_fft = fft(signal); % FFT of the signal
    plot(freq_axis, abs(signal_fft(1:N_fft))); % Plot
end

% Parks-McClellan LP filter
function [result, delay] = pm_lp_filter(signal, F_s, freq_range, dev)
    [n, fo, ao, w] = firpmord(freq_range, [1 0], [dev dev], F_s);
    b = firpm(n, fo, ao, w);
    
    delay = ceil(mean(grpdelay(b, 1)));
    result = filter(b, 1, signal);
    result(1:delay) = [];
end
