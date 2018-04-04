function [ new_cell_array ] = concatenateCellArrays( cell_array, new_element )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

len_cell_array = size(cell_array, 2);

cell_array {len_cell_array +1} = new_element;

new_cell_array = cell_array;

end

