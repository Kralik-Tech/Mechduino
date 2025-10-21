function instal_Mechduino()
    % addpath("+controllers\")
    % addpath("+examples\")
    % cd(fileparts(which(mfilename)))
    filePath = fileparts(which(mfilename));
    addpath(genpath(filePath));
    savepath
    % disp('<a href="http://mechlab.cz">Dokumentace</a>')
end
