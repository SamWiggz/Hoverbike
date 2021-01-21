<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#getting-started">Getting Started</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

Hoverbike EE175A/B Fall 2020-Winter 2021

<!-- Getting Started -->
## Getting Started

Initial Starting Guide
1. Clone Project
  - Create new folder for this project on your computer
  - Use Gitbash/Command Prompt/Terminal and CD into your new folder
  - Copy and paste: (`git clone https://github.com/SamWiggz/Hoverbike.git`) into terminal
2. Create your Local Branch 
  - Copy and paste: (`git checkout -b <branch name>`) into terminal
  - Local branch used to code/test your new changes/features
  - Test on your local branch then push to master

Push Local Changes to Main Branch
1. Sync Master branch
  - (`git checkout main`)
  - (`git pull`)
2. Rebase Local Branch with Master Branch
  - (`git checkout -b <branch name>`)
  - (`git rebase origin/master`)
3. Commit Changes from Local Branch
  - (`git commit -m '<example change description>'`)
4. Push to the Main Branch
  - (`git push origin <branch name>`)
5. Open a Pull Request
  
