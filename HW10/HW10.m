function data = HW10(port)

%% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
fprintf('Opening port %s....\n',port);
mySerial = serial(port, 'BaudRate', 230400); 
fopen(mySerial); % opens serial connection
clean = onCleanup(@()fclose(mySerial)); % closes serial port when function exits

%% Sending Data
% Printing to matlab Command window
fprintf(mySerial,'r\n');

%% Reading data
fprintf('Waiting for samples ...\n');

sampnum = 1; % index for number of samples read
read_samples = 0; % When this value from PIC32 equals 1, it is done sending data
while read_samples < 100
    data_read = fscanf(mySerial,'%d %d %d %d %d'); % reading data from serial port
    
    % Extracting variables from data_read
    read_samples=data_read(1);
    raw(sampnum)=data_read(2);
    MAF(sampnum)=data_read(3);
    IIR(sampnum)=data_read(4);
    FIR(sampnum)=data_read(5);
    
    sampnum=sampnum+1; % incrementing loop number
end

data = [raw,MAF,IIR,FIR]; % setting data variable

%% Plotting data
clf;
hold on;
t = 1:sampnum-1;
plot(t,raw);
plot(t,MAF);
plot(t,IIR);
plot(t,FIR);
legend('Raw', 'MAF', 'IIR', 'FIR')
title('Filtered data');
ylabel('Acceleration in the z direcetion');
xlabel('Sample Number (at 100 Hz)');
hold off;
end
