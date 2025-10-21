clear; clc;
coms = serialportlist("available");

timeout_seconds = 10; % časový limit pro každý port

if ~isempty(coms)
    for i = 1:length(coms)
        fprintf("Zkouším port: %s\n", coms(i));
    
        % Spuštění úlohy na pozadí
        port = parfeval(@testPortSafe, 1, coms(i));
        tStart = tic;
        isDone = false;
        
        while toc(tStart) < timeout_seconds
            if strcmp(port.State, "finished") || strcmp(port.State, "error")
                isDone = true;
                break;
            end
            pause(0.1);
        end
        
        if isDone
            try
                result = fetchOutputs(port);
                fprintf("Výstup z portu %s: %s\n", coms(i), result);
            catch ME
                fprintf("Chyba při čtení výsledku z portu %s: %s\n", coms(i), ME.message);
            end
        else
            cancel(port);
            fprintf("Timeout na portu %s\n", coms(i));
        end
        delete(port);
        clear port;
    end

end

clear
