
#include "SceneScript.hpp"

SceneScript::SceneScript(){
    Chapter ch0 = {1, 0, "You find yourself at the foot of a mountain. \"To Experience\", read a notice on a wooden board in front of you.\n\nPress A to start the journey"};
    
    Chapter ch1 = {2, 1, "It's not hard to start climbing a mountain. The views are beautiful and the air is fresh. You enjoy the nature so much...\n\nPress A to continue"};

    Chapter ch2 = {3, 4, "You suddenly feel a cool breeze and you hear the water sounds flowing over rocks. You see a lake in the distance. Someone is sitting by the lake...\n\nPress A to go to the lake. Press D to continue"};
    
    Chapter ch3 = {5, 3, "He's also a climber, just like you. His name is Robert. You have a lot of things in common. You have a good chat and decide to continue the journey together...\n\nPress A to continue"};

    Chapter ch4 = {6, 4, "You continue to climb. It's becoming harder because it's higher...\n\nPress A to continue"};

    Chapter ch5 = {9, 5, "You continue to climb. It's becoming harder because it's higher. But you still feel very happy and relaxed with the company of your friend...\n\nPress A to continue"};

    Chapter ch6 = {7, 8, "You see a cat in the grass. You approach it and it doesn't move but only tremble. You find out that it's injured by something. Do you want to try to treat it?\n\nPress A to treat. press D to leave"};

    Chapter ch7 = {8, 7, "Because of your treatment, the cat is getting better. You smile at it and then continue your journey. \n\nPress A to continue"};

    Chapter ch8 = {13, 8, "The weather is turning cold gradually. You start to feel harder every day after 2 hours of climbing.  But you're still curious about what it will be like to get to the top of the mountain.\n\nPress A to continue"};
    Chapter ch9 = {10, 9, "The weather is turning cold gradually. You start to feel hard every day after 2 hours of climbing.  But you're still curious about what it will be like to get to the top of the mountain. However, one day Robert tells you that he wants to give up. \n\nPress A to continue"};
    Chapter ch10 = {11, 12, "\"You wanna leave with me?\" Robert ask.\n\nPress A to leave. Press D to continue"};
    Chapter ch11 = {20, 11, "You go down the hill together. It's a good time to go with your friend. You feel he's the most important person to you. But sometimes you still wonder what if you climb to the top of the mountain.\n\nPress A to continue"};
    Chapter ch12 = {13, 12, "Robert left. You feel sad but you still want to continue your journey.\n\nPress A to continue"};
    Chapter ch13 = {21, 13, "It's becoming more and more steep and difficult. You know that because you can only climb a shorter distance with a longer time. You even hurt yourself because of some grass with sharp leaves and hard rocks. You start to feel loss.\n\nPress A to continue"};
    Chapter ch14 = {20, 14, "You go down the hill. You feel so relieved. Everything is enough for you. Though you didn't climb to the top, It still was a great experience. But sometimes you still wonder what if you climb to the top of the mountain.\n\nPress A to continue"};
    Chapter ch15 = {16, 15, "When you decided to continue the climbing, you suddenly have a feeling that it's not so important to climb to the top. You started this journey because you enjoy climbing, right? It's great to reach the top, but it doesn't matter if you couldn't. You feel relieved.\n\nPress A to continue"};
    Chapter ch16 = {17, 16, "You don't know how many days and nights you've spent on climbing. You just keep climbing. You see dark conifers, snow and ice lakes and you hear fewer sounds of birds or animals.\n\nPress A to continue"};
    Chapter ch17 = {18, 17, "One day, you stop climbing at midnight. You are so tired that you fall asleep. Several hours later you wake up suddenly, with sunshine on your face. You open up your eyes and find that you're almost on the top! There're only several steps to the top!\n\nPress A to continue"};
    Chapter ch18 = {19, 18, "You finally reach the top. You get the most beautiful views of what you've seen. You feel so happy and enjoy the views.\n\nPress A to continue"};
    Chapter ch19 = {20, 19, "But you see another mountain, right behind you. It's higher and tougher. You freeze for a while and then you smile.\n\nPress A to continue"};
    Chapter ch20 = {0, 20, "~The end~\n\nPress A to restart"};
    Chapter ch21 = {15, 14, "Why did you start this journey? You're lost in thoughts.\n\nYou know this is the time to make the final decision.\n\nPress A to continue Press D to go down the mountain."};

    stories.push_back(ch0);
    stories.push_back(ch1);
    stories.push_back(ch2);
    stories.push_back(ch3);
    stories.push_back(ch4);
    stories.push_back(ch5);
    stories.push_back(ch6);
    stories.push_back(ch7);
    stories.push_back(ch8);
    stories.push_back(ch9);
    stories.push_back(ch10);
    stories.push_back(ch11);
    stories.push_back(ch12);
    stories.push_back(ch13);
    stories.push_back(ch14);
    stories.push_back(ch15);
    stories.push_back(ch16);
    stories.push_back(ch17);
    stories.push_back(ch18);
    stories.push_back(ch19);
    stories.push_back(ch20);
    stories.push_back(ch21);

    cur_chapter = ch0;
}