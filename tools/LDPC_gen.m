clear,clc 

%% Getting input from user

message_len = input("Enter message length: ");
if isempty(message_len) 
    error("No value entered!"); 
end

if mod(message_len, 8)
    error("Message length must be divisible by 8")
end

parity_bits = input("Enter number of parity bits: ");
if isempty(parity_bits) 
    error("No value entered!");
end

if (message_len / parity_bits <= 0.5 ) || (parity_bits < 5)
    error("Too little parity bits");
end


seed = input("Enter seed for random generator: ");
if isempty(seed) 
    error("No value entered!");
end

%% Setting up parity check matrix generation

rng(seed, "twister");

%% Creating parity check matrix

parity_check_matrix_rand = 2 * abs(randn(parity_bits, message_len));

parity_check_matrix_base = zeros(parity_bits, message_len);

max_ones = ceil(log(message_len));

% Adding ones
for i = 1:parity_bits
    one_num = 1 + ceil(abs(randn()));
    [~, indexes] = maxk(parity_check_matrix_rand(i, :), one_num);
    parity_check_matrix_base(i, indexes) = 1;
end

% Adding ones if there is less than two in collum
for i = 1:message_len
    if nnz(parity_check_matrix_base(:, i) < 2)
        [~, indexes] = maxk(parity_check_matrix_rand(:, i), 2);
        parity_check_matrix_base(indexes, i) = 1;
    end
end

% Adding ones if there is less than two in row
for i = 1:parity_bits
    if nnz(parity_check_matrix_base(:, i) < 2)
        [~, indexes] = maxk(parity_check_matrix_rand(i, :), 2);
        parity_check_matrix_base(i, indexes) = 1;
    end
end

% Check if parity checks are independent
for i = 1:parity_bits
    [~, indexes_i] = maxk(parity_check_matrix_base(i, :), nnz(parity_check_matrix_base(i, :)));
   
    j = 1;

    
    while j < parity_bits
        if j == i  % Not comparing parity check with itself
            j = j + 1;

            continue
        end


        num_of_nonzero_elements = nnz(parity_check_matrix_base(j, :));

        [~, indexes_j1] = maxk(parity_check_matrix_base(j, :), num_of_nonzero_elements);       
        unique_bits = ~ismember(indexes_i, indexes_j1);

        if nnz(unique_bits)
            j = j + 1;

            continue;
        end

        [~, indexes_j0] = mink(parity_check_matrix_base(j, :), message_len - num_of_nonzero_elements);   

        changed1 = randi(num_of_nonzero_elements);
        changed0 = randi(message_len - num_of_nonzero_elements);

        parity_check_matrix_base(j, indexes_j1(changed1)) = 0;
        parity_check_matrix_base(j, indexes_j0(changed0)) = 1;

        j = 1;

    end

end

% making matrix more sparsely populated
for i = 1:parity_bits
    while nnz(parity_check_matrix_base(i, :)) > max_ones
        [~, ones_indexes] = maxk(parity_check_matrix_base(i, :), nnz(parity_check_matrix_base(i, :)));

        most_populated_collum = [0 0]; %index 1 is number of collum, index 2 is number of ones in that collum
        
        for j = 1:numel(ones_indexes)
            ones_in_collum = nnz(parity_check_matrix_base(:, ones_indexes(j)));

            if ones_in_collum > most_populated_collum(2)
                most_populated_collum = [ones_indexes(j), ones_in_collum];
            end
        end

        if most_populated_collum(2) <= 2
            break;
        end

        parity_check_matrix_base(i, most_populated_collum(1)) = 0;
    end
end

parity_check_matrix = [parity_check_matrix_base, eye(parity_bits)];

%% Creating generating matrix

generating_matrix = parity_check_matrix_base';

%% Output

generating_matrix_output_full = false;

output_format = input("Output generating matrix in full format (including identity matrix part)? [Y/N] ", "s");

if isempty(output_format) 
    disp("No value entered! Defaulting to No");
    output_format = 'N';
end

output_format = upper(output_format);

if output_format == 'Y'
    generating_matrix = [eye(message_len), generating_matrix];
    generating_matrix_output_full = true;
end

prompt = input("Output as c variables? [Y/N] ", "s");

if isempty(prompt) 
    disp("No value entered! Defaulting to No");
    prompt = 'N';
end

prompt = upper(prompt);

if prompt == 'Y'
    [filename, location] = uiputfile('output.c', "Select output file");
    if filename == 0
        error("No output file selected");   
    end

    file = fopen(append(location, filename), 'w');

    fprintf(file, "%s\n\n%s\n\n", "#include <stdlib.h>", "#include <stdint.h>");

    offset_start = ftell(file);

    % Outputing generating matrix

    bytes_per_row = ceil(message_len / 8);

    fprintf(file, "%s%d%s%d%s\n", "uint8_t generating_matrix[", message_len + generating_matrix_output_full * parity_bits, "][", bytes_per_row, "] = {" );

    bracket_offset = repelem(' ', ftell(file) - 1 - offset_start);

    for i = 1:parity_bits + generating_matrix_output_full * parity_bits
        fprintf(file, "%s%s", bracket_offset, "{");

        row_data = resize(generating_matrix(:, i), bytes_per_row * 8 + 1);

        for j = 1:bytes_per_row
            fprintf(file, "%s", "0b");
            fprintf(file, "%d",row_data(1:8));

            row_data = row_data(9:end);

            if j < bytes_per_row
                fprintf(file, "%s", ", ");
            end
        end

        fprintf(file, "%s", "}");

        if i ~= parity_bits + generating_matrix_output_full * parity_bits
            fprintf(file, "%s\n", ",");
        end
    end

    fprintf(file, "\n%s\n\n", "};");

    % Outputing parity check matrix

    offset_start = ftell(file);

    bytes_per_row = ceil((message_len + parity_bits) / 8);

    fprintf(file, "%s%d%s%d%s\n", "uint8_t parity_check_matrix[", parity_bits, "][", bytes_per_row, "] = {" );

    bracket_offset = repelem(' ', ftell(file) - 1 - offset_start);

    for i = 1:parity_bits
        fprintf(file, "%s%s", bracket_offset, "{");

        row_data = resize(parity_check_matrix(i, :), message_len + parity_bits + ceil(bytes_per_row * 8 - message_len - parity_bits + 1));

        for j = 1:bytes_per_row
            fprintf(file, "%s", "0b");
            fprintf(file, "%d",row_data(1:8));

            row_data = row_data(9:end);

            if j < bytes_per_row
                fprintf(file, "%s", ", ");
            end
        end

        fprintf(file, "%s", "}");

        if i ~= parity_bits
            fprintf(file, "%s\n", ",");
        end
    end

    fprintf(file, "\n%s\n\n", "};");

    % Outputing Tanner graph hints

    hints = zeros(parity_bits, message_len);
    hints_number = zeros(1, parity_bits);

    hints_max_dim = 0;

    for i = 1:parity_bits
        [~, hints_in_row] = maxk(parity_check_matrix(i, :), nnz(parity_check_matrix(i, :)));

        hints_number(i) = numel(hints_in_row);

        if (hints_number(i) > hints_max_dim)
            hints_max_dim = hints_number(i);
        end

        for j = 1:hints_number(i)
            hints(i, j) = hints_in_row(j) - 1;
        end
    end
    
    fprintf(file, "%s%d%s", "uint8_t tanner_graph_hints_per_parity_bit[", parity_bits,"] = {");

    for i = 1:parity_bits
        fprintf(file, "%d", hints_number(i));

        if i ~= parity_bits
            fprintf(file, "%s", ", ");
        end
    end

    fprintf(file, "%s\n\n", "};");

    offset_start = ftell(file);

    fprintf(file, "%s%d%s%d%s\n", "uint8_t tanner_graph_hints[", parity_bits, "][", hints_max_dim, "] = {");

    bracket_offset = repelem(' ', ftell(file) - 1 - offset_start);

    for i = 1:parity_bits
        fprintf(file, "%s%s", bracket_offset, "{");

        for j = 1:hints_max_dim
            fprintf(file, "%d", hints(i, j));

            if j ~= hints_max_dim
                fprintf(file, "%s", ", ");
            end
        end

        fprintf(file, "%s", "}");

        if i ~= parity_bits
            fprintf(file, "%s\n", ",");
        end
    end

    fprintf(file, "\n%s", "};");

else
    [filename, location] = uiputfile('output.txt', "Select output file");
    if filename == 0
        error("No output file selected");
    end

    file = fopen(append(location, filename), 'w');

    fprintf(file, "%s\n\n", "Generating matrix:");

    for i = 1:message_len
        fprintf(file, "%d ", generating_matrix(i, :));

        fprintf(file, "\n");
    end

    fprintf(file, "\n%s\n\n", "Parity check matrix:");

    for i = 1:parity_bits
        fprintf(file, "%d ", parity_check_matrix(i, :));

        fprintf(file, "\n");
    end
end
