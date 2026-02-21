# Christopher Poole
# IT - 140 Text Based Game
import os  # used to operate cls




# Greeting message
def show_instructions():
    # print main message and instructions
    print('You have been escorted into a dark room by armed guards. They leave, locking the door.'.center(89, '-'))
    print('After a few moments the lights click on and you hear a voice over the intercom:'.center(90, '-'))
    print('"D-class personnel, enter the containment area.'.center(90, '-'))
    print('Equip the 8 pieces of protective equipment found in each room.'.center(90, '-'))
    print('You will then travel to the Northeast corridor and interact with SCP-1048-A.'.center(90, '-'))
    print(''
          '"'.center(90, '-'))
    print('On a nearby table, you see a haggard sketch of the area.'.center(90, '-'))
    print('\n')

    # map visual
    map_item = '0'  # denote objects to avoid errors involving tuples
    wall = '*'
    enemy_room = '*  O  *  O  *  X  *'
    room = '*  O  *  O  *  O  *'
    entrance_room = '*******-- --*******'
    while wall != 'q':
        print((wall * 19).center(90, ' '))  # wall
        print(enemy_room.format(map_item).center(90, ' '))
        print((wall * 19).center(90, ' '))  # wall
        print(room.format(map_item).center(90, ' '))
        print((wall * 19).center(90, ' '))  # wall
        print(room.format(map_item).center(90, ' '))
        print(entrance_room.center(90, ' '))  # wall
        print('\n')
        break

    print("Move commands: Go North, Go South, Go East, Go West.".center(90, '-'))
    print("Equip item: Equip 'item name'.".center(90, '-'))
    # print("Exit game: Type 'Exit'.".center(90, '-'))

    input("Enter any key to continue...".center(90, '-'))


def clear():  # Portability
    os.system('cls' if os.name == 'nt' else 'clear')  # non-Windows

# map dictionary


rooms = {
    'Entrance': {'North': 'South Room'},
    'South Room': {'East': 'Southeast Room', 'West': 'Southwest Room', 'item': 'Quantum Pants'},
    'Southeast Room': {'West': 'South Room', 'North': 'East Room', 'item': 'Quantum Gloves'},
    'East Room': {'West': 'Center Room', 'North': 'Northeast Room', 'South': 'Southeast Room', 'item': 'Quantum Boots'},
    'Center Room': {'North': 'North Room', 'East': 'East Room', 'item': 'Quantum Earplugs'},
    'North Room': {'West': 'Northwest Room', 'South': 'Center Room', 'item': 'Quantum Battery Pack'},
    'Northwest Room': {'South': 'West Room', 'East': 'North Room', 'item': 'Quantum Coveralls'},
    'West Room': {'North': 'Northwest Room', 'South': 'Southwest Room', 'item': 'Quantum Helmet'},
    'Southwest Room': {'North': 'West Room', 'East': 'South Room', 'item': 'Quantum Vest'},
    'Northeast Room': {'South': 'East Room', 'Boss': 'SCP-1048-A'}
    }

# List that tracks equipment
equipment = []

# Tracks current room
current_room = 'Entrance'

# List containing last move to be appended
msg = " "

# List of vowels
vowels = ['a', 'e', 'i', 'o', 'u']

clear()
show_instructions()

# Gameplay loop
while True:

    # clears terminal after each iteration
    clear()

    # Info display
    print(f"You are in the {current_room}\nEquipment : {equipment}\n{'-' * 30}")

    # Display msg
    print(msg)

    # Item check
    if "item" in rooms[current_room].keys():

        room_item = rooms[current_room]["item"]

        if room_item not in equipment:

            # Plural block specific for this game
            if room_item[-1] == 's':
                print(f"You see {room_item}")

            elif room_item[0] in vowels:
                print(f"You see an {room_item}")

            else:
                print(f"You see a {room_item}")

    # Boss
    if "Boss" in rooms[current_room].keys():

        # lose condition
        if len(equipment) < 8:
            print(f"A lonely teddy bear stands in front of you. It is made of human ears.".center(90, '-'))
            print('You hear a high-pitched shriek.'.center(90, '-'))
            print(f"Pain erupts from every orifice. Ears begin to grow all over your body.".center(90, '-'))
            print(f'The last thing you recognize is the voice over the intercom:'.center(90, '-'))
            print(f'"Test 4-ac complete. Subject terminated."'.center(90, '-'))
            print('\n')

            print('You died. Game Over.'.center(90, '-'))
            break

        # win condition
        else:
            print(f"You recognize the shape of a teddy bear in front of you. It is made of human ears.".center(90, '-'))
            print(f"The bear appears to vibrate, but you cannot hear anything.".center(90, '-'))
            print(f'After some time, a voice comes over the intercom inside your helmet:'.center(90, '-'))
            print(f'"D-Class personnel, return to the entrance. Today, you have earned your freedom."'.center(90, '-'))
            print('\n')
            print('You leave Site-24 alive, and as a free person. You Win!!!'.center(90, '-'))
            break

    # Player's move
    user_input = input("Enter your move:\n")

    # Split move
    next_move = user_input.split(' ')

    # First word
    action = next_move[0].title()  # title case to accept any case

    direction = 0
    item = 0
    if len(next_move) > 1:
        item = next_move[1:]  # List slice to get any words following first
        direction = next_move[1].title()  # Always first word

        item = ' '.join(item).title()  # used to join multiple items

    # Moving between rooms
    if action == "Go":  # [0] 1st acceptable input

        try:
            current_room = rooms[current_room][direction]
            msg = f"You travel {direction}."

        except:
            msg = f"You can't go that way."

    # Picking up items
    elif action == "Equip":  # [0] 2nd acceptable input

        try:
            if item == rooms[current_room]["item"]:

                if item not in equipment:

                    equipment.append(rooms[current_room]["item"])
                    msg = f"{item} equipped"

                else:
                    msg = f"You already have the {item} equipped."

            else:
                msg = f"Can't find {item}."

        except:
            msg = f"Can't find {item}."

    # Exit game
    # elif action == "Exit":
        # break

    # Any other commands invalid
    else:
        msg = "Invalid command"
