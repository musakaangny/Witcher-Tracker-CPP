default:
	g++ -std=c++11 -o witchertracker src/main.cpp src/Potion.cpp src/Beast.cpp src/Inventory.cpp src/Bestiary.cpp src/AlchemyKnowledge.cpp src/CommandParser.cpp src/WitcherTracker.cpp

clean:
	rm -f witchertracker

grade:
	python3 test/grader.py ./witchertracker test-cases
