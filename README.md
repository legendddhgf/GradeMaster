# GradeMaster
A grading system intended to make every aspect of grading more convenient

## Expected features
* System entirely configurable without recompiling / changing the code
* Easy to use with complete documentation
* Enable tracking of grader for every individual section per student
* having prespecified comments with grades associated
* TBD

## Current system order of operations
1. Begin loading information from configuration files
1. Apply any command-line flags, overriding default configurations if applicable
1. Clear id's for all pre-existing entries in the global section_info-id hash table by setting them to ~0 (TODO: why)
1. For every section in the current assignment's configuration
    1. Perform a check to see if there is a saved section that matches the one being observed, if not then an entry is added to the global section_info-id hash table accordingly
    1. This id becomes the index of the new entry for this section created in the assignment sections vector
1. System in shell mode
    1. Further details TBD

## Ideas undergoing consideration (No form to this section whatsoever)
* Have a remote mode where you can run the system as a server on the system with the files to be graded
and as a remote client on the system where one prefers working.
    * will be looking into daemon() so the server side runs itself as a background process
* How to handle midway changes to sections.
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
                * At the same time, if we would like to allow a section to have it's descriptions modified while keeping saved grades intact, how can one "uniquely identify" a given section?
                * Currently considering: for every section the system observes in the configuration file that hasn't before been considered, it will automatically attempt to determine its status
                    * there will be a global hash map of sec_ids to hashes of the section information, if something doesn't match existing info, it will auto determine
                        * note that this hash map is a possible solution to the problem of having grades pointing to invalid sections on delete.
                    * After auto determine is completed, it will prompt the user confirmation.
                    * For example, a similarity algorithm can be run to determine if the section is actually just a string modification of a previous section.
                    * In the case that the user prompts that it is in fact not a modification of an old section, options such as "new section", or "modification of other section" can be prompted.
                    * In the end, the aforementioned global hash map of "sec_ids to hashes of section information" can simply be updated
* Need to look into similarity benchmarks.
    * This isn't for cheat checking, which is already handled by moss (moss checking functionality can be added to this system though).
    * Rather, this is for coming up with a quantitative "hash" of grading section information to be used in comparing an existing section hash with that of a new section. Details of why this is necessary are spread throughout the above section.
* an "exception" system which excludes certain things in certain cases (I feel like this will be an innate property of the system rather than an explicit structure/sub-library)
* August's ideas:
    * 
