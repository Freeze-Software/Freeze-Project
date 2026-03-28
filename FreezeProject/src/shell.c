#include "shell.h"

#include "input.h"
#include "rtc.h"
#include "vga.h"
#include "fs.h"
#include "disk.h"
int startswith(const char* s, const char* p) {
    int i = 0;
    while (p[i]) {
        if (s[i] != p[i]) return 0;
        i++;
    }
    return 1;
}
int strcmp(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}
void shell() {
    char buf[128];
    while(1){
        print("\033[95mFreeze-OS>\033[0m ");
        get_input(buf);
        if (startswith(buf, "sudo ")) {
            print("[sudo] ");
            char* rest = buf + 5;
            handle_command(rest);
        } else if (startswith(buf, "freeze ")) {
            print("[freeze] ");
            char* rest = buf + 5;
            handle_command(rest);
        } else {
            handle_command(buf);
        }
    }
}

// commands
void handle_command(char* buf) {
    if (strcmp(buf, "help") == 1) {
        print("Built in commands:\n");
        print(
            "uname, date, id, who, ps, top, lsmod, systemctl, shutdown, "
            "apps\n");
        print("echo, sed, awk, wc, head\n");
        print("ls, pwd, file, stat, chown, ln, du\n");
        print("kill, exit, sleep\n");
        print("useradd, groups, sudo\n");
        print(" make, bash, sh, man, which, whereis\n");
        print("clear, about, version, info, test, reboot, hl\n");
        print("\nFile System Commands (Persistent):\n");
        print("edit <name>, cat <name>, rm <name>, save <name>, fsync\n");
    } else if (strcmp(buf, "clear") == 1) {
        clear();
    } else if (strcmp(buf, "-r") == 1) {
        print("Safety implementations deny this action.\n");
        print_hex((unsigned int)&__bss_start);
        print(" - ");
        print_hex((unsigned int)&__bss_end);
        print("\n");
    } else if (strcmp(buf, "about") == 1) {
        print("The FreezeOS Is an operating system created by Clashnewbme.\n");
    } else if (strcmp(buf, "apps") == 1) {
        print("FreezeProject/applications/talktoyourself.fp\n");
        print("FreezeProject/applications/typingtest.fp\n");
        print("FreezeProject/applications/adarkplace.fp\n");
        print("FreezeProject/applications/library.fp\n");
        print(
            "To run these applications just type their file name so to run "
            "typing test type, (typingtest.fp) this will run the app\n");
        print(
            "This rule does not apply for library, just type (library) and the "
            ".fp will auto run.\n");
    } else if (strcmp(buf, "fork while forking") == 1) {
        print("Forking while forking...\n");
        print("Forking while forking...\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, "version") == 1) {
        print("Freeze Project 0.64\n");
    } else if (strcmp(buf, "date") == 1) {
        int sec, min, hour, day, mon, year;

        read_rtc(&sec, &min, &hour, &day, &mon, &year);

        // CMOS validation
        if (mon < 1 || mon > 12) {
            print("RTC error\n");
            return;
        }

        print(months[mon - 1]);
        putc(' ');

        print_int(day);
        putc(' ');

        print_int(year);
        putc(' ');

        print_2digit(hour);
        putc(':');
        print_2digit(min);
        putc(':');
        print_2digit(sec);

        print(" UTC\n");
    } else if (strcmp(buf, "flipped date") == 1) {
        int year, min, hour, day, mon, sec;

        read_rtc(&year, &min, &hour, &day, &mon, &sec);

        if (mon < 1 || mon > 12) {
            print("RTC error\n");
            return;
        }

        print(months[mon - 1]);
        putc(' ');

        print_int(day);
        putc(' ');

        print_int(year);
        putc(' ');

        print_2digit(hour);
        putc(':');
        print_2digit(min);
        putc(':');
        print_2digit(sec);

        print(" UTC\n");
    } else if (strcmp(buf, "id") == 1) {
        print("uid=0(root) gid=0(root) groups=0(root),4(adm),27(sudo)\n");
    } else if (strcmp(buf, "who") == 1) {
        print("root     pts/0        2026-02-26 05:00 (:0)\n");
    } else if (strcmp(buf, "ps") == 1) {
        print("PID USER COMMAND\n1   root kernel\n2   root systemd\n");
    } else if (strcmp(buf, "top") == 1) {
        print("PID %%CPU %%MEM COMMAND\n");
        print("1   Unknown    Unknown    kernel\n");
    } else if (strcmp(buf, "lsmod") == 1) {
        print("Module       Size\nserial_core  2048\n");
    } else if (strcmp(buf, "systemctl") == 1) {
        print("Usage: systemctl [start|stop|status|restart] <service>\n");
    } else if (strcmp(buf, "shutdown") == 1) {
        fs_sync();
        print("Shutting down...\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, "ls") == 1) {
        fs_list();
    } else if (strcmp(buf, "open editor") == 1) {
        print("Opening editor....\n");
        print("\033[96m=== \033[95mfile.fp Editor\033[96m ===\033[0m\n");
        print("\033[92mText file succesfully created\033[0m\n");
        print("\033[93mUse type to write\033[0m\n\n");
        print("\033[94m--------------------------------\033[0m\n");
        print("\033[92mText file typer: \033[93mVersion 0.35\033[0m\n");
    } else if (startswith(buf, "edit ")) {
        char* filename = buf + 5;
        int fd = fs_find(filename);
        if (fd < 0) {
            fd = fs_create(filename);
            if (fd < 0) {
                print("Cannot create file\n");
            }
        }
        print("Editing ");
        print(filename);
        print(":\n");
        get_input(buf);
        int len = 0;
        while (buf[len]) len++;
        fs_write(fd, buf, len);
        fs_sync();
        print("Saved to disk\n");
    } else if (strcmp(buf, "stat") == 1) {
        print("File: kernel.bin Size: 2.5\n");
    } else if (strcmp(buf, "chown") == 1) {
        print("File owner changed\n");
    } else if (strcmp(buf, "ln") == 1) {
        print("Creating symlink...\n");
    } else if (strcmp(buf, "fsync") == 1) {
        fs_sync();
        print("File system synced to disk\n");
    } else if (startswith(buf, "rm ")) {
        char* filename = buf + 3;
        if (fs_delete(filename) == 0) {
            print("File deleted: ");
            print(filename);
            print("\n");
        } else {
            print("Cannot delete file\n");
        }
    } else if (startswith(buf, "save ")) {
        char* filename = buf + 5;
        int fd = fs_find(filename);
        if (fd >= 0) {
            fs_save(fd);
            print("File saved to disk: ");
            print(filename);
            print("\n");
        } else {
            print("File not found\n");
        }
    } else if (startswith(buf, "cat ")) {
        char* filename = buf + 4;
        int fd = fs_find(filename);
        if (fd >= 0) {
            char buffer[MAX_FILE_SIZE + 1];
            int read = fs_read(fd, buffer, MAX_FILE_SIZE);
            buffer[read] = 0;
            print(buffer);
            print("\n");
        } else {
            print("File not found\n");
        }
    } else if (strcmp(buf, "echo") == 1) {
        print("Type something:\n");
        get_input(buf);
        print(buf);
        putc('\n');
    } else if (startswith(buf, "echo ")) {
        print(buf + 5);
        putc('\n');
    } else if (strcmp(buf, "talktoyourself.fp") == 1) {
        print("Opening FreezeProject/applications/talktoyourself.fp\n");
        print("        \n");
        print("        \n");
        print("\033[92m");
        char i = 0;
    while (i < 2) {

        print("\033[92m");

        print(
            "01010100 01101000 01100101 00100000 01000110 01110010 01100101 "
            "01100101 01111010 01100101\n");
        print(
            "01010000 01110010 01101111 01101010 01100101 01100011 01110100 "
            "00100000 00110000 00110001\n");
        print(
            "11100010 10011000 10000101 00100000 01010011 01111001 01110011 "
            "01110100 01100101 01101101\n");
        print(
            "00000000 11111111 10101010 01010101 11001100 00110011 11110000 "
            "00001111 10100101 01011010\n");
        print(
            "01010110 10101001 00111100 11000011 01100110 10011001 01000010 "
            "00100100 01111110 10000001\n");

        print("\033[0m");

        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }
        print("\033[0m");
        print("        \n");
        print("        \n");
        print("you> ");
        get_input(buf);
        print(buf);
        putc('\n');
    } else if (startswith(buf, "you> ")) {
        print(buf + 5);
        putc('\n');
    } else if (strcmp(buf, "typingtest.fp") == 1) {
        print("Opening FreezeProject/applications/typingtest.fp\n");
    char i = 0;
    while (i < 5) {

        print("\033[92m");

        print(
            "01010100 01101000 01100101 00100000 01000110 01110010 01100101 "
            "01100101 01111010 01100101\n");
        print(
            "01010000 01110010 01101111 01101010 01100101 01100011 01110100 "
            "00100000 00110000 00110001\n");
        print(
            "11100010 10011000 10000101 00100000 01010011 01111001 01110011 "
            "01110100 01100101 01101101\n");
        print(
            "00000000 11111111 10101010 01010101 11001100 00110011 11110000 "
            "00001111 10100101 01011010\n");
        print(
            "01010110 10101001 00111100 11000011 01100110 10011001 01000010 "
            "00100100 01111110 10000001\n");

        print("\033[0m");

        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }
        print("\033[0m");
        print("Welcome to THE Typing Test!\n");
        print("Type the word as shown.\n");
        print("        \n");

        print("Word: supercalifragilisticexpialidocious\n");
        print("you> ");
        get_input(buf);

        if (strcmp(buf, "supercalifragilisticexpialidocious") == 1) {
            print("Correct!\n");
        } else {
            print(
                "Wrong! The fully correct word is "
                "'supercalifragilisticexpialidocious'\n");
        }

        print("        \n");

        print("Word: pseudopseudohypoparathyroidism\n");
        print("you> ");
        get_input(buf);

        if (strcmp(buf, "pseudopseudohypoparathyroidism") == 1) {
            print("Correct!\n");
        } else {
            print(
                "Wrong! The correct word is "
                "'pseudopseudohypoparathyroidism'\n");
        }

        print("        \n");

        print("Word: antidisestablishmentarianism\n");
        print("you> ");
        get_input(buf);

        if (strcmp(buf, "antidisestablishmentarianism") == 1) {
            print("Correct!\n");
        } else {
            print(
                "Wrong! The correct word was 'antidisestablishmentarianism'\n");
        }

        print("        \n");
        print("Game Over!\n");

    } else if (strcmp(buf, "adarkplace.fp") == 1) {
        print("Opening FreezeProject/applications/adarkplace.fp\n");
    char i = 0;
    while (i < 70) {

        print("\033[92m");

        print(
            "01010100 01101000 01100101 00100000 01000110 01110010 01100101 "
            "01100101 01111010 01100101 00100000 01010000 01110010 01101111 "
            "01101010 01100101 01100011 01110100\n");
        print(
            "01010100 01101000 01100101 00100000 01000110 01110010 01100101 "
            "01100101 01111010 01100101 00100000 01010000 01110010 01101111 "
            "01101010 01100101 01100011 01110100\n");

        print("\033[0m");

        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }
        print("\033[0m");
        print("        \n");
        print("\033[95m=== A dark place ===\033[0m\n");
        print("Type to choose your path\n");
        print("        \n");

        print("\033[96mYou wake up in a dark place.\033[0m\n");
        print("Go 'forward' or 'stay'?\n");
        print("you> ");
        get_input(buf);

        if (strcmp(buf, "stay") == 1) {
            print("\033[90mYou wait... nothing happens...\033[0m\n");
            print("\033[91mYou fade away.\033[0m\n");
        }

        else if (strcmp(buf, "forward") == 1) {
            print("\033[96mYou walk forward and find a shadow.\033[0m\n");
            print("Do you 'fight' or 'run'?\n");
            print("you> ");
            get_input(buf);

            if (strcmp(buf, "run") == 1) {
                print("\033[94mYou escape safely.\033[0m\n");
                print("\033[92mYou survive.\033[0m\n");
            }

            else if (strcmp(buf, "fight") == 1) {
                // SCENE 3
                print("\033[91mThe shadow attacks!\033[0m\n");
                print("Type 'attack' to strike\n");
                print("you> ");
                get_input(buf);

                if (strcmp(buf, "attack") == 1) {
                    print("\033[93mYou hit the shadow!\033[0m\n");

                    // FINAL CHOICE
                    print("\033[91mIt strikes back!\033[0m\n");
                    print("Type 'attack' again or 'dodge'\n");
                    print("you> ");
                    get_input(buf);

                    if (strcmp(buf, "dodge") == 1) {
                        print("\033[92mYou dodged and survived!\033[0m\n");
                    }

                    else if (strcmp(buf, "attack") == 1) {
                        print("\033[91mYou trade hits...\033[0m\n");
                        print("\033[91mYou fall.\033[0m\n");
                    }

                    else {
                        print("You hesitate...\n");
                        print("\033[91mThe shadow consumes you.\033[0m\n");
                    }
                }

                else {
                    print("You failed to act...\n");
                    print("\033[91mThe shadow consumes you.\033[0m\n");
                }
            }

            else {
                print("Invalid choice.\n");
            }
        }

        else {
            print("Invalid choice.\n");
        }

        print("        \n");
        print("\033[95m=== END ===\033[0m\n");

    } else if (strcmp(buf, "library") == 1) {
        print("Opening FreezeProject/applications/library.fp\n");
        print("\033[92m");
    char i = 0;
    while (i < 50) {

        print("\033[92m");

        print(
            "01010100 01101000 01100101 00100000 01000110 01110010 01100101 "
            "01100101 01111010 01100101 00100000 01010000 01110010 01101111 "
            "01101010 01100101 01100011 01110100\n");

        print("\033[0m");

        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }
        print("        \n");
        print("\033[95m=== FREEZE PROJECT LIBRARY ===\033[0m\n");
        print("Choose a book (1)\n");
        print("Books:\n");
        print("1: Frankenstein\n");
        print("2: All Good Things Have to End Sometime\n");
        print("3: Winter’s Whisper.\n");

        int running = 1;

        while (running) {
            print("\033[96mLibrary> \033[0m");
            get_input(buf);
            // hey reminder to contributor, we are using the
            // https://www.gutenberg.org/ free library, please do not use or add
            // copyrighted material.
            if (strcmp(buf, "1") == 1) {
                print("\033[93mFrankenstein - Mary Shelley\033[0m\n");

                print("Letter 1\n");
                print("To Mrs. Saville, England.\n\n");
                print("St. Petersburgh, Dec. 11th, 17—.\n");
                print(
                    "You will rejoice to hear that no disaster has accompanied "
                    "the commencement of an enterprise which you have regarded "
                    "with such evil forebodings.\n");
                print(
                    "I arrived here yesterday, and my first task is to assure "
                    "my dear sister of my welfare and increasing confidence in "
                    "the success of my undertaking.\n");
                print(
                    "I am already far north of London, and as I walk in the "
                    "streets of Petersburgh, I feel a cold northern breeze "
                    "play upon my cheeks...\n");

                print("\nLetter 2\n");
                print("Archangel, 28th March, 17—.\n");
                print(
                    "How slowly the time passes here, encompassed as I am by "
                    "frost and snow!\n");
                print("Yet a second step is taken towards my enterprise...\n");

                print("\nLetter 3\n");
                print("July 7th, 17—.\n");
                print("My dear Sister,\n");
                print(
                    "I write a few lines in haste to say that I am safe—and "
                    "well advanced on my voyage...\n");

                print("\nLetter 4\n");
                print("August 5th, 17—.\n");
                print(
                    "So strange an accident has happened to us that I cannot "
                    "forbear recording it...\n");

                print("\nChapter 1\n");
                print(
                    "I am by birth a Genevese, and my family is one of the "
                    "most distinguished of that republic.\n");
                print(
                    "My ancestors had been for many years counsellors and "
                    "syndics...\n");
                print(
                    "My father, Alphonse Frankenstein, was respected by all "
                    "who knew him...\n");
                print(
                    "He came to the assistance of a merchant friend who had "
                    "fallen into poverty...\n");
                print(
                    "Among his friends was one who had been reduced from "
                    "affluence to ruin...\n");
                print("This man's daughter was Caroline Beaufort...\n");
                print("My father sought her out and married her...\n");
                print("\033[93mFrankenstein - Mary Shelley\033[0m\n");

                print("\nChapter 2\n");
                print(
                    "We were brought up together; there was not quite a year "
                    "difference in our ages.\n");
                print(
                    "I need not say that we were strangers to any species of "
                    "disunion or dispute.\n");
                print(
                    "Harmony was the soul of our companionship, and the "
                    "diversity and contrast that subsisted in our characters "
                    "drew us nearer together.\n");
                print(
                    "Elizabeth was of a calmer and more concentrated "
                    "disposition; but, with all my ardour, I was capable of a "
                    "more intense application.\n");
                print(
                    "She busied herself with following the aerial creations of "
                    "the poets; and in the majestic and wondrous scenes which "
                    "surrounded our Swiss home.\n");
                print(
                    "The world was to me a secret which I desired to "
                    "divine.\n");
                print(
                    "Curiosity, earnest research to learn the hidden laws of "
                    "nature, gladness akin to rapture, as they were unfolded "
                    "to me, are among the earliest sensations I can "
                    "remember.\n");
                print(
                    "When I was about thirteen years of age, we all went on a "
                    "party of pleasure to the baths near Thonon.\n");
                print(
                    "The weather was fine; and we passed several days "
                    "happily.\n");
                print(
                    "One day, while wandering among the mountains, I found a "
                    "volume of the works of Cornelius Agrippa.\n");
                print(
                    "I opened it with apathy; the theory which he attempts to "
                    "demonstrate and the wonderful facts which he relates soon "
                    "changed this feeling into enthusiasm.\n");
                print("A new light seemed to dawn upon my mind...\n");

                print("\nChapter 3\n");
                print(
                    "When I had attained the age of seventeen, my parents "
                    "resolved that I should become a student at the University "
                    "of Ingolstadt.\n");
                print(
                    "I had hitherto attended the schools of Geneva, but my "
                    "father thought it necessary for the completion of my "
                    "education that I should be made acquainted with other "
                    "customs than those of my native country.\n");
                print("My departure was therefore fixed at an early date.\n");
                print(
                    "But before the day resolved upon could arrive, the first "
                    "misfortune of my life occurred—an omen, as it were, of my "
                    "future misery.\n");
                print(
                    "Elizabeth had caught the scarlet fever; her illness was "
                    "severe, and she was in the greatest danger.\n");
                print(
                    "During her illness, many arguments had been urged to "
                    "persuade my mother to refrain from attending upon her.\n");
                print(
                    "She had at first yielded to our entreaties; but when she "
                    "heard that the life of her favourite was menaced, she "
                    "could no longer control her anxiety.\n");
                print(
                    "She attended her sickbed; her watchful attentions "
                    "triumphed over the malignity of the distemper.\n");
                print(
                    "Elizabeth was saved, but the consequences of this "
                    "imprudence were fatal to her preserver.\n");
                print(
                    "On the third day my mother sickened; her fever was "
                    "accompanied by the most alarming symptoms.\n");
                print(
                    "On her deathbed, she joined the hands of Elizabeth and "
                    "myself.\n");
                print(
                    "“My children,” she said, “my firmest hopes of future "
                    "happiness were placed on the prospect of your union.”\n");
                print("She died calmly...\n");

                print("\nChapter 4\n");
                print(
                    "From this day natural philosophy, and particularly "
                    "chemistry, in the most comprehensive sense of the term, "
                    "became nearly my sole occupation.\n");
                print(
                    "I read with ardour those works, so full of genius and "
                    "discrimination, which modern inquirers have written on "
                    "these subjects.\n");
                print(
                    "I attended the lectures and cultivated the acquaintance "
                    "of the men of science of the university.\n");
                print("In M. Waldman I found a true friend.\n");
                print(
                    "He praised with warmth the astonishing progress I had "
                    "made, and exhorted me to persevere.\n");
                print(
                    "Under his guidance I entered with the greatest diligence "
                    "into the search of the philosopher’s stone and the elixir "
                    "of life.\n");
                print("But these were not my only visions.\n");
                print(
                    "The raising of ghosts or devils was a promise liberally "
                    "accorded by my favourite authors...\n");
                print(
                    "Life and death appeared to me ideal bounds, which I "
                    "should first break through, and pour a torrent of light "
                    "into our dark world.\n");
                print(
                    "A new species would bless me as its creator and "
                    "source.\n");
                print(
                    "Many happy and excellent natures would owe their being to "
                    "me.\n");
                print(
                    "No father could claim the gratitude of his child so "
                    "completely as I should deserve theirs.\n");
                print(
                    "Pursuing these reflections, I thought, that if I could "
                    "bestow animation upon lifeless matter, I might in process "
                    "of time renew life where death had apparently devoted the "
                    "body to corruption.\n");

                print("\033[93mGenerating book...\033[0m\n");

                print("\nChapter 5\n");
                print(
                    "It was on a dreary night of November that I beheld the "
                    "accomplishment of my toils.\n");
                print(
                    "With an anxiety that almost amounted to agony, I "
                    "collected the instruments of life around me.\n");
                print(
                    "It was already one in the morning; the rain pattered "
                    "dismally against the panes, and my candle was nearly "
                    "burnt out.\n");
                print(
                    "By the glimmer of the half-extinguished light, I saw the "
                    "dull yellow eye of the creature open.\n");
                print(
                    "It breathed hard, and a convulsive motion agitated its "
                    "limbs.\n");
                print(
                    "How can I describe my emotions at this catastrophe, or "
                    "how delineate the wretch whom with such infinite pains "
                    "and care I had endeavoured to form?\n");
                print(
                    "His limbs were in proportion, and I had selected his "
                    "features as beautiful.\n");
                print("Beautiful!—Great God!\n");
                print(
                    "His yellow skin scarcely covered the work of muscles and "
                    "arteries beneath.\n");
                print(
                    "His hair was of a lustrous black, and flowing; his teeth "
                    "of a pearly whiteness.\n");
                print(
                    "But these luxuriances only formed a more horrid contrast "
                    "with his watery eyes...\n");
                print(
                    "Unable to endure the aspect of the being I had created, I "
                    "rushed out of the room...\n");

                print("\nChapter 6\n");
                print(
                    "Clerval called forth the better feelings of my heart; he "
                    "again taught me to love the aspect of nature.\n");
                print(
                    "I had been the cause of so much misery, and I shrank from "
                    "the remembrance.\n");
                print(
                    "I seized the letter that Clerval had brought from my "
                    "father.\n");
                print("It was from Elizabeth.\n");
                print(
                    "“My dearest cousin,” she wrote, “you have been ill, very "
                    "ill, and even the constant letters of dear Henry are not "
                    "sufficient to reassure me on your account.”\n");
                print(
                    "She described the happiness of my family and the tranquil "
                    "life they led.\n");
                print(
                    "She spoke also of Justine Moritz, who had returned to our "
                    "house...\n");
                print(
                    "“She is very clever and gentle,” wrote Elizabeth, “and "
                    "extremely pretty.”\n");
                print(
                    "These cheerful tidings did much to restore my spirits.\n");
                print(
                    "Spring advanced rapidly; the weather became fine and "
                    "serene...\n");

                print("\nChapter 7\n");
                print(
                    "On my return, I found the following letter from my "
                    "father:—\n");
                print("“My dear Victor,\n");
                print(
                    "You have probably waited impatiently for a letter to fix "
                    "the date of your return to us.”\n");
                print("“William is dead!”\n");
                print(
                    "That sweet child, whose smiles delighted and warmed my "
                    "heart, who was so gentle, yet so gay!\n");
                print("He was murdered.\n");
                print(
                    "Come, dearest Victor; you alone can console "
                    "Elizabeth.”\n");
                print("I shuddered at the calamity.\n");
                print(
                    "I saw the necessity of returning to Geneva, and I "
                    "immediately set out.\n");
                print(
                    "As I approached my native town, I saw the lightning "
                    "playing on the summit of Mont Blanc.\n");
                print("The storm appeared to approach rapidly.\n");
                print(
                    "In a moment of sudden illumination, I beheld the figure "
                    "of a man at some distance.\n");
                print(
                    "A flash of lightning illuminated the object, and "
                    "discovered its shape plainly to me.\n");
                print(
                    "Its gigantic stature, and the deformity of its aspect, "
                    "more hideous than belongs to humanity, instantly informed "
                    "me that it was the wretch—the filthy daemon to whom I had "
                    "given life.\n");
                print(
                    "I considered the being whom I had cast among mankind, and "
                    "endowed with the will and power to effect purposes of "
                    "horror...\n");
                print("\033[93mFrankenstein - Mary Shelley\033[0m\n");

                print("\nChapter 8\n");
                print(
                    "We passed a few sad hours until eleven o'clock, when the "
                    "trial was to commence.\n");
                print(
                    "My father and the rest of the family being obliged to "
                    "attend as witnesses, I accompanied them to the court.\n");
                print(
                    "During the whole of this wretched mockery of justice I "
                    "suffered living torture.\n");
                print(
                    "It was to be decided whether the result of my curiosity "
                    "and lawless devices would cause the death of two of my "
                    "fellow beings.\n");
                print("Justine was called.\n");
                print(
                    "She appeared calm, yet her calmness was evidently "
                    "constrained.\n");
                print("Many witnesses were called against her.\n");
                print(
                    "She had been seen near the spot where William had been "
                    "murdered.\n");
                print(
                    "A picture of my mother, which William had worn, was found "
                    "in her pocket.\n");
                print("The evidence was strong, and she was convicted.\n");
                print("“I did confess,” she said, “but I confessed a lie.”\n");
                print(
                    "She spoke with a firm voice, yet tears streamed from her "
                    "eyes.\n");
                print("“God knows how entirely I am innocent.”\n");
                print("The judges passed the sentence of death.\n");
                print("I listened in horror.\n");
                print("The tortures of the accused did not equal mine.\n");

                print("\nChapter 9\n");
                print(
                    "Nothing is more painful to the human mind than a great "
                    "and sudden change.\n");
                print("The fall from prosperity to misery was too abrupt.\n");
                print(
                    "I remained two months in Geneva, incapable of attending "
                    "to any occupation.\n");
                print("Remorse extinguished every hope.\n");
                print("I was the true murderer.\n");
                print("Elizabeth was sad and desponding.\n");
                print(
                    "“The death of William, the execution of Justine, and all "
                    "the misery that has followed,” she said, “have deprived "
                    "me of repose.”\n");
                print(
                    "My father tried to inspire us with hope, but the loss was "
                    "too heavy.\n");
                print(
                    "I avoided the face of man; all sound of joy or "
                    "complacency was torture to me.\n");
                print(
                    "At length I resolved to seek solace in the mountains.\n");
                print("I journeyed to Chamounix.\n");
                print(
                    "The vast and magnificent scenes afforded me the greatest "
                    "consolation that I was capable of receiving.\n");

                print("\nChapter 10\n");
                print(
                    "I spent the following day roaming through the valley.\n");
                print(
                    "The immense mountains and precipices that overhung me, "
                    "the sound of the river raging among the rocks, and the "
                    "dashing of the waterfalls spoke of a power mighty as "
                    "Omnipotence.\n");
                print(
                    "These sublime and magnificent scenes afforded me the "
                    "greatest consolation.\n");
                print("They elevated me from all littleness of feeling.\n");
                print(
                    "But the sight of the monster filled me with bitterness "
                    "and hatred.\n");
                print(
                    "He approached; his countenance bespoke bitter anguish.\n");
                print("“Devil,” I exclaimed, “do you dare approach me?”\n");
                print("“Begone, vile insect!”\n");
                print("He replied calmly, “I expected this reception.”\n");
                print(
                    "“All men hate the wretched; how, then, must I be hated, "
                    "who am miserable beyond all living things!”\n");
                print("He demanded that I hear his tale.\n");
                print(
                    "“Listen to my story,” he said, “and then abandon or "
                    "commiserate me, as you shall judge that I deserve.”\n");
                print("I hesitated, but at length I consented to hear him.\n");
                print("\033[93mFrankenstein - Mary Shelley\033[0m\n");

                print("\nChapter 11\n");
                print(
                    "The creature begins his story, describing his early "
                    "experiences.\n");
                print(
                    "He awakens alone, confused, and overwhelmed by "
                    "sensations.\n");
                print(
                    "He learns through observation—light, heat, hunger, and "
                    "sleep.\n");
                print(
                    "He discovers fire, first bringing comfort, then pain.\n");
                print(
                    "He begins to understand his environment and his own "
                    "existence.\n");
                print("Loneliness becomes his greatest suffering.\n");

                print("\nChapter 12\n");
                print(
                    "The creature wanders through forests and eventually finds "
                    "a small hut attached to a cottage.\n");
                print(
                    "He secretly observes the De Lacey family living "
                    "inside.\n");
                print(
                    "He watches their interactions and begins to understand "
                    "human behavior.\n");
                print(
                    "He notices kindness, love, and communication among "
                    "them.\n");
                print("Through them, he learns language and emotions.\n");
                print(
                    "He feels admiration for their goodness, but also deep "
                    "sadness about his own isolation.\n");

                print("\nChapter 13\n");
                print(
                    "The creature continues observing the De Lacey family "
                    "closely.\n");
                print(
                    "He learns more about language and begins to speak to "
                    "himself.\n");
                print(
                    "He discovers that the family is poor, yet kind and "
                    "hardworking.\n");
                print(
                    "He becomes especially interested in their situation and "
                    "hopes to eventually reveal himself.\n");
                print(
                    "He begins to form a plan to approach them, believing they "
                    "may accept him.\n");
                print(
                    "Despite his hopes, he still struggles with his appearance "
                    "and fear of rejection.\n");
            }

            // hey reminder to contributors for this story, we are using the
            // https://reedsy.com/short-story/l2t862/ story, please do not use
            // or add copyrighted material.
            if (strcmp(buf, "2") == 1) {
                print("\033[93mAll Good Things Have to End Sometime\033[0m\n");

                print("By Nicholas LeRouge\n");
                print(
                    "Santa Claus’s announcement was greeted with stunned "
                    "silence.\n");
                print(
                    "“All good things have to end sometime,” he said. “I’m "
                    "old, I’m tired. I can’t go on for ever.” Then he "
                    "chuckled, “Although it already feels like I’ve been doing "
                    "this for ever.”\n");
                print(
                    "“But you can’t stop! No!!” some children said. Others "
                    "were in tears.\n");
                print(
                    "“Sorry, kids, I’m done. It’s incredibly exhausting to be "
                    "responsible for delivering all your gifts on the one "
                    "day.”\n");
                print("“But… who’ll do your job instead?”\n");
                print(
                    "“You’ll find someone else. They’ll do things their way, "
                    "differently, but just as well.”\n");
                print("​Then, with a final goodbye, he faded out.\n");
            }
            // This story has absolutly no license, you may use or do anything
            // you want with it ig.
            if (strcmp(buf, "3") == 1) {
                print("\033[41mWinter’s Whisper.\033[0m\n");

                print("By Edward James\n");
                print("Snowflakes drifted quietly through the night,\n");
                print(
                    "each one unique, tracing delicate patterns in the cold "
                    "air.\n");
                print("A child reached out, catching a fleeting crystal,\n");
                print("watching it melt into nothing.\n");
                print("In that moment, she learned beauty can be brief\n");
                print(
                    "but its memory lingers like winter’s whisper long after "
                    "it disappears softly.\n");
            }

            else if (strcmp(buf, "exit") == 1) {
                print("Closing library...\n");
                running = 0;
            }

            else {
                print("Unknown selection.\n");
            }
        }

        print("        \n");
        print("\033[95mLibrary closed.\033[0m\n");
    } else if (strcmp(buf, "kill") == 1 || strcmp(buf, "kill all") == 1) {
        print("Successfully killed all processes\n");
        print("Restarting to process changes\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, "hlr") == 1) {
        print("\033[41m");
        print(
            "                            -- Highlight color red Selected --    "
            "                       \n");
    } else if (strcmp(buf, "hlb") == 1) {
        print("\033[44m");
        print(
            "                         -- Highlight color Blue Selected --      "
            "                          \n");
    } else if (strcmp(buf, "hlm") == 1) {
        print("\033[45m");
        print(
            "                        -- Highlight color Magenta Selected --    "
            "                         \n");

    } else if (strcmp(buf, "hlg") == 1) {
        print("\033[42m");
        print(
            "                        -- Highlight color Green Selected --      "
            "                       \n");

    } else if (strcmp(buf, "sleep") == 1) {
        print("Sleeping...\n");
        outb(0x64, 0xFE);
    } else if (strcmp(buf, "exit") == 1) {
        print("Exiting...\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, "useradd") == 1) {
        print("Unable to do so.\n");
    } else if (strcmp(buf, "colors") == 1) {
            print("Warning! Flasing colors..");
    char i = 0;
    while (i < 150) {

        print("\033[31m&&& \033[32m&&& \033[33m&&& \033[34m&&& \033[35m&&&\n");
        print("\033[36m&&& \033[31m&&& \033[32m&&& \033[33m&&& \033[34m&&&\n");
        print("\033[35m&&& \033[36m&&& \033[31m&&& \033[32m&&& \033[33m&&&\n");
        print("\033[34m&&& \033[35m&&& \033[36m&&& \033[31m&&& \033[32m&&&\n");
        print("\033[33m&&& \033[34m&&& \033[35m&&& \033[36m&&& \033[31m&&&\n");
        print("\033[0m\n");

        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }
    } else if (strcmp(buf, "Install /image/colored-sky") == 1) {
        print(
            "\033[34m&&& \033[34m&&& \033[34m&&& \033[31m&&& \033[33m&&& "
            "\033[34m&&&\n");
        print(
            "\033[34m&&& \033[33m&&& \033[31m&&& \033[34m&&& \033[34m&&& "
            "\033[33m&&&\n");
        print(
            "\033[32m&&& \033[32m&&& \033[30m&&& \033[30m&&& \033[31m&&& "
            "\033[32m&&&\n");
        print(
            "\033[32m&&& \033[30m&&& \033[31m&&& \033[30m&&& \033[32m&&& "
            "\033[32m&&&\n");
        print(
            "\033[31m&&& \033[33m&&& \033[31m&&& \033[33m&&& \033[31m&&& "
            "\033[33m&&&\n");
        print(
            "\033[30m&&& \033[30m&&& \033[31m&&& \033[30m&&& \033[30m&&& "
            "\033[31m&&&\n");
        print("\033[0m\n");
    } else if (strcmp(buf, "Install /image/room") == 1) {
        print(
            "\033[37m&&& \033[37m&&& \033[37m&&& \033[37m&&& \033[37m&&& "
            "\033[37m&&& \033[37m&&& \033[37m&&&\n");
        print(
            "\033[37m&&& \033[37m&&& \033[33m&&& \033[37m&&& \033[33m&&& "
            "\033[37m&&& \033[37m&&& \033[37m&&&\n");

        print(
            "\033[34m&&& \033[34m&&& \033[36m&&& \033[36m&&& \033[36m&&& "
            "\033[34m&&& \033[34m&&& \033[34m&&&\n");
        print(
            "\033[34m&&& \033[36m&&& \033[36m&&& \033[33m&&& \033[36m&&& "
            "\033[36m&&& \033[34m&&& \033[34m&&&\n");
        print(
            "\033[34m&&& \033[36m&&& \033[33m&&& \033[33m&&& \033[33m&&& "
            "\033[36m&&& \033[36m&&& \033[34m&&&\n");
        print(
            "\033[34m&&& \033[36m&&& \033[36m&&& \033[33m&&& \033[36m&&& "
            "\033[36m&&& \033[36m&&& \033[34m&&&\n");

        print(
            "\033[33m&&& \033[33m&&& \033[33m&&& \033[31m&&& \033[31m&&& "
            "\033[33m&&& \033[33m&&& \033[33m&&&\n");
        print(
            "\033[33m&&& \033[31m&&& \033[31m&&& \033[31m&&& \033[31m&&& "
            "\033[31m&&& \033[31m&&& \033[33m&&&\n");

        print(
            "\033[30m&&& \033[30m&&& \033[30m&&& \033[33m&&& \033[30m&&& "
            "\033[30m&&& \033[30m&&& \033[30m&&&\n");
        print(
            "\033[30m&&& \033[33m&&& \033[30m&&& \033[33m&&& \033[30m&&& "
            "\033[33m&&& \033[30m&&& \033[30m&&&\n");

        print(
            "\033[30m&&& \033[30m&&& \033[33m&&& \033[30m&&& \033[30m&&& "
            "\033[33m&&& \033[30m&&& \033[30m&&&\n");
        print(
            "\033[30m&&& \033[33m&&& \033[30m&&& \033[33m&&& \033[30m&&& "
            "\033[30m&&& \033[33m&&& \033[30m&&&\n");

        print(
            "\033[33m&&& \033[33m&&& \033[33m&&& \033[33m&&& \033[33m&&& "
            "\033[33m&&& \033[33m&&& \033[33m&&&\n");

        print("\033[0m\n");
    } else if (strcmp(buf, "Dev") == 1) {
        print("\033[96m=== \033[95mFreeze Project\033[96m ===\033[0m\n");
        print("\033[92mhttps://freezeos.org/\033[0m\n");
        print(
            "\033[93mDeveloped by @Clashnewbme, @Crystal_Nitr0, and "
            "others.\033[0m\n\n");
        print("\033[94m--------------------------------\033[0m\n");
        print("\033[92mCurrently: \033[93mVersion 0.64\033[0m\n");
    } else if (strcmp(buf, "sh") == 1) {
        print("POSIX shell\n");
    } else if (strcmp(buf, "freezefetch") == 1) {
        print("\n");

        print("\033[36m");
        print("        *        \n");
        print("       ***       ------------\n");
        print("      ** **      OS: FreezeOS\n");
        print("   * ** * ** *   Kernel: x86\n");
        print("    ***   ***    Version 0.64\n");
        print(" * ** * * * ** * \n");
        print("  **   ***   **  \n");
        print(" * ** * * * ** * \n");
        print("    ***   ***    \n");
        print("   * ** * ** *   \n");
        print("      ** **      \n");
        print("       ***       \n");
        print("        *        \n");
        print("   ==         == \n");
        print("\033[0m");

        print("\n");

        print("   ");
        print("\033[40m   \033[0m");
        print("\033[100m   \033[0m");
        print("\033[47m   \033[0m");
        print("\033[97m   \033[0m");
        print("\033[46m   \033[0m");
        print("\033[44m   \033[0m");
        print("\033[106m   \033[0m");
        print("\033[107m   \033[0m");

        print("\n\n");
    } else if (strcmp(buf, "true") == 1) {
        print("\n");
    } else if (strcmp(buf, "false") == 1) {
        print("\n");
    } else if (strcmp(buf, "info") == 1 || strcmp(buf, "kernel") == 1 ||
               strcmp(buf, "test") == 1) {
        print("Freeze OS v0.64\n");
        print("Created by Clashnewbme and Crystal_Nitr0\n");
    } else if (strcmp(buf, "FreezeOS") == 1 || strcmp(buf, "freezeos") == 1 ||
               strcmp(buf, "Freeze") == 1 || strcmp(buf, "freeze") == 1) {
        print("Freeze\n");
    } else if (strcmp(buf, ":(){:|:&};:") == 1) {

    while (1) {

        print("Forking :(){:|:&};:...\n");

        for (volatile long d = 0; d < 10000000; d++);
    }
    } else if (strcmp(buf, "Import /chkrootkit/*") == 1) {
        print("Denied\n");
        print("Restarting System\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, "reboot") == 1) {
        print("Rebooting...\n");
        outb(0x64, 0xFE);
        for (;;);
    } else if (strcmp(buf, ":).sss") == 1) {

    char i = 0;
    while (i < 150) {

            print("\033[31m   ██  \n");
            print("██  ██  \n");
            print("    ██  \n");
            print("    ██  \n");
            print("██  ██  \n");
            print("   ██   \033[0m\n");
        for (volatile long d = 0; d < 10000000; d++);

        i++;
    }

        print("\n:)\n");

    } else if (strcmp(buf, "freezeme") == 1) {

    while (1) {
        print("\033[92m");

        print(
            "11000011 00111100 10101001 01010110 01100110 10011001 11100011 "
            "00011100 01010101 10101010 00110011 11001100 00001111 11110000 "
            "01111110 10000001 01000010 00100100\n");

        print("\033[0m");

        for (volatile long d = 0; d < 10000000; d++);
    }



    } else {
        print("Command not found.\n");
    }
}
