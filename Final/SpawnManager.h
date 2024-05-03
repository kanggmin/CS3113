/*
pseudo code

class spawnmanager {

private:

	total enemies killed
	enemy respawn rate

	//enemy spawn stats
	hp
	speed


public:

	initialise {
		initalise all areas
		spawn in max number of enemies in random positions in every area
		initialise enemy spawn stats
	}
	
	update {
		check if min enemies in area if true set area to fill
		if fill and not max, start timer for area spawns, and start spawning
		update all enemies

		//maybe scale enemies based on total kills?
	}

	render {
		render all enemies
		render hp hearts above enemies
	}

	class area {
	
	public:
		vector of glm::vec3 of spawnpoints in area
		vector of entities //for enemies in area
		float of enemies alive in area
		floats of x and y border of area for enemy detection
		int of max enemies in area
		int of min enemies before starting respawns
		time since respawn in area
		boolean fill spawns
	}
}
*/

/*
OTHER THINGS TO DO STILL

CREATE ENEMIES
	FAST EXPLODING 1HP *EFFECT

IMPLEMENT ENEMY DROPPING HEARTS

POSSIBLY:
SCALE ENEMIES
SCALE PLAYER WITH LEVELS
CREATE BOSS
CREATE 3 OBJ TO FIND IN THE AREAS TO WIN GAME

OR 3 LITTLE MAPS WITH FIND AN OBJ, KILL ENEMIES AND ENTER PORTAL
*/
