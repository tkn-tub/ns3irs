function cleaned_matrix = clear_low_gain_values(matrix, percentile)
    % Calculate the threshold based on the percentile
    sorted_values = sort(matrix(:));
    threshold_index = round(length(sorted_values) * percentile / 100);
    threshold = sorted_values(threshold_index);

    [rows, cols] = size(matrix);
    cleaned_matrix = matrix;

    for i = 1:rows
        for j = 1:cols
            if matrix(i,j) <= threshold
                % Find neighbors
                neighbors = [];
                for ni = max(1,i-1):min(rows,i+1)
                    for nj = max(1,j-1):min(cols,j+1)
                        if (ni ~= i || nj ~= j) && matrix(ni,nj) > threshold
                            neighbors = [neighbors, matrix(ni,nj)];
                        end
                    end
                end
                % Replace with mean of neighbors if any valid neighbors exist
                if ~isempty(neighbors)
                    cleaned_matrix(i,j) = mean(neighbors);
                end
            end
        end
    end
end
