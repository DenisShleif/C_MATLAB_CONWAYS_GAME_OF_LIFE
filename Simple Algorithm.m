function game(worldX,worldY,time)
    world = round(rand(worldY,worldX)); %Create a world randomly filledwith live cells 
    for time = 1:time %iterate through time
        tmpWorld = world; %store the world into a temporary world
        for p = 1:worldY %iterate through all rows of world
            for q = 1:worldX %iterate through all columns of world
                numNeighbours = sum(sum(tmpWorld((p - (p ~= 1)) : (p + (p ~= worldY)), (q - (q ~= 1)) : (q + (q ~= worldX))))); %Calculates the number of neighbours of cell p,q
                world(p,q) = ((numNeighbours == 3 || numNeighbours == 4) && tmpWorld(p,q)) || ( numNeighbours == 3 && ~world(p,q)); %if the cell is alive and has 3 or 4 neighbours or if it is dead and has 3 neighbors make in alive in world
            end
        end
    spy(world); %plot the world
    pause(0.05); %standard pause between iterations
end