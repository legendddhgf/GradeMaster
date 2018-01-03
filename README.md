# GradeMaster
A grading system intended to make every aspect of grading more convenient

## Expected features
* System entirely configurable without recompiling / changing the code
* Easy to use with complete documentation
* Enable tracking of grader for every individual section per student
* having prespecified comments with grades associated
* scripts that not only can run if they are associated with your section responsibilities
    * but also those that can run alongside to automatically populate grades and comments
* csv export customizable format
* grade.txt customizability
* intermediate email routines (i.e: initially send out files submitted, then maybe cheaters, finally all grades)
    * emails templatable
* temp folder for students currently being graded (i.e under temp_graderID)
* format in JSON (likely) or YAML
* add assignment sections via internal CLI prompt rather than through manual file editing
* critical / most configuration handled internally, some can be handled externally
* enforce configuration is consistent?
* customizable export of system configuration
* split window to show output and terminal at the same time side by side
* follow unix standard for interface?
* TBD

## Expected system order of operations
1. Begin loading information from configuration files
1. Apply any command-line flags, overriding default configurations if applicable
1. Clear id's for all pre-existing entries in the global section_info-id hash table by setting them to ~0 (TODO: why)
1. For every section in the current assignment's configuration
    1. Perform a check to see if there is a saved section that matches the one being observed
    1. if not then an entry is added to the global section_info-id hash table accordingly
    1. This id becomes the index of the new entry for this section created in the assignment sections vector
1. System in shell mode
    1. Further details TBD

## Current Progress (described both current implementation and near-future planned implementation)
* Structure for containing student info
    * holds full name
    * holds an abbreviated name (in UCSC this will probably be a cruzID of the student)
    * a vector of grades represented as percentages for every section (there will be as many entries in the vector as there are sections) or ~0 (255) as ungraded. Note: this may have to be changed slightly to reflect more degrees of specificity in grades (i.e by using floating point vars)
    * TODO: potentially another vector to mark which grader did each section (since sometimes 2 graders may work on the same section)
* Structure for containing grader info
    * holds full name
    * holds an abbreviated name (in UCSC this will probably be a cruzID of the student)
    * holds a bitvector with a 1 marked for every section the grader is responsible for (contains an entry for every section). Note this may be changed especially in accordance to requirements for allowing multiple graders to grade the same section although I think this may be handled with a vector of graders for each section in the student info structure.
* Structure for containing configuration info (This will contain all information required for GradeMaster to function). Currently undergoing heavy changes under @4U6U57
    * Since configuration changes are common and may even change over the course of versions, it was determined that a HashTable would be the most reasonable structure for containing all configuration options which will likely be of two forms:
        * option & <TRUE|FALSE>
        * param & <VAL>
    * The configuration info will mostly be populated through the GradeMaster interface as we are trying to prevent the manual editing of files as much as possible
* A Makefile
    * It's fancy?
* a BitVector data structure
    * it's a vector of bits
* A interface
    * Being worked on by @4U6U57
* A file containing the main functionality (will be called by interface)
    * Being worked on by @legendddhgf
* A main file to call everything else

## Ideas undergoing consideration (No form to this section whatsoever)
* version control?
* Have remote/client modes
* Think about how to effectively reflect grades (particularly after regrades) in the system: currently considering hashing all section info into ID
    * It's one thing to change the name but if the section is unrelated, then there are a number of options:
        * delete the grades for the section if it's information is removed (dangerous, what if somebody was just trying to add a section and deleted a bunch of their work)
        * save the grades for the section even if it's removed (now you end up having a bunch of files that may not be necessary but there are other complications)
            * what do you save them under?
                * the sections no longer exists so they would basically be pointing to outdated data, ...waiting for the section to exist again?
        * tracking which grades belong to which section
            * if there are three sections, and another section is added after section 1 (because it logically fits there), then how do section 2 and 3 continue to point to the correct information?
            * Currently considering: internally automatically assign an id to each section. Have a global id counter for every assignment. Use a hash map between sec_id and percentage per section.
            * However, what allows the system to uniquely identify a section?
                * It would be ridiculous to require the user to number them when that can be done automatically.
                * At the same time, if we would like to allow a section to have its descriptions modified while keeping saved grades intact, how can one "uniquely identify" a given section?
                * Currently considering: for every section the system observes in the configuration file that hasn't before been considered, it will automatically attempt to determine its status
                    * there will be a global hash map of sec_ids to hashes of the section information, if something doesn't match existing info, it will auto determine
                        * note that this hash map is a possible solution to the problem of having grades pointing to invalid sections on delete.
                    * After auto determine is completed, it will prompt the user for confirmation.
                    * For example, a similarity algorithm can be run to determine if the section is actually just a string modification of a previous section.
                    * In the case that the user prompts that it is in fact not a modification of an old section, options such as "new section", or "modification of other section" can be prompted.
                    * In the end, the aforementioned global hash map of "sec_ids to hashes of section information" can simply be updated
* Need to look into similarity benchmarks.
    * This isn't for cheat checking, which is already handled by moss (moss checking functionality can be added to this system though).
    * Rather, this is for coming up with a quantitative "hash" of grading section information to be used in comparing an existing section hash with that of a new section. Details of why this is necessary are spread throughout the above section.
* an "exception" system which excludes certain things in certain cases (I feel like this will be an innate property of the system rather than an explicit structure/sub-library)
