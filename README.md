# ezecs
EZ Entity Component System
---

#### TL;DR This is an automatic entity component system generator that probably isn't fit for public consumption, unless you find it an interesting project.
#### Ezecs is an attempt to make the creation and use of a custom (and fast) entity component system a breeze. It does this by allowing the user to configure a single C++ header, from which is generated hundreds of lines of code that would normally be very tedious to manage. Previous attempts to acheive this via C macros and templating became intractable and were never very cross-platform (see how MSVC handles \_\_VA_ARGS\_\_ for example). Thanks to CMake, however, integrating source code generation into a build system is now easer than ever, so I'm giving this another go.

---
#### Ezecs is very much a work-in-progress, and is not yet suitable for basically anyone but me to use, mostly just due to the lack of robustness of the C++ parsing I'm doing right now.
#### What I'm discovering is that what I really need is a proper C++ parser to make this system robust. I wanted to avoid "just using json," because it just doesn't provide the flexibility I want without imposing what amounts to yet another arcane meta-language.
#### As it stands, the configuration file can be pretty finnicky. Usable, but not yet "EZ".

