function result = testPortSafe(com)
    result = "Mechduino tu je";
    try
        port = serialport(com, 9600, "Timeout", 1);
        pause(1);
        write(port, [3, 4, 248], "uint8");
        pause(0.5);
        r = read(port, 3, "uint8")
        if isempty(r)
           result = "Zde nic";
        end
        delete(port);
        clear port
    catch ME
        result = "Chyba: " + ME.message;
    end
end
