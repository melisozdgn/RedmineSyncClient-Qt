# RedmineSyncClient-Qt (Qt & C++)

This is my internship project developed with C++ and the Qt Framework. The main goal was to build a desktop client for Redmine that follows an **"Offline-First"** approach, allowing users to keep working even without an active internet connection.

##  What I Built
 
** Offline Access ** I used `QJsonDocument` to create a local storage system. When the app starts, it loads everything from a local `tasks.json` file first, so the interface is ready immediately without waiting for a server response.
* **Smart Sync Logic:** I implemented the communication with Redmine's REST API using `QNetworkAccessManager`. When you click "Synchronize", the app handles the complex task of pulling new updates and pushing local changes back to the server.
* **Conflict Resolution:**  I designed a `ConflictDialog` that detects if a task was modified both locally and on the server. It shows both versions side-by-side so the user can decide which data to keep.
* **Gantt Chart & Progress Tracking:** To make the data more visual, I added a custom **Gantt Chart** for timelines and integrated **Progress Bars** into the list view to track task completion at a glance.

##  Architecture

I used the **MVC (Model-View-Controller)** pattern to keep the code organized:

* **Model:** Structured data classes for Issues and Tasks.
* **View:** The main dashboard, specialized dialogs, and the Gantt visualization.
* **Controller/Service:** The "engine" room that handles API requests (`RedmineManager`) and local file saving (`TaskRepository`).

##  Environment & Setup

* **Server Side:** I tested the application against a local Redmine instance running on **Docker (localhost:8090)**.
* **Development:** Built with **C++17** and **Qt 6** using Qt Creator.
* **Run:** Open `RedmineSyncProject.pro`, ensure your Docker container is up, and just hit Run.

## 🎓 Learning Outcomes

This internship was a great opportunity to dive deep into professional software development. My key takeaways include:

* **REST API Integration:** Learning how to handle asynchronous HTTP requests (GET/PUT) and manage API authentication.
* **Data Serialization:** Mastering JSON parsing and serialization in C++ to manage local persistence.
* **UI Design in Qt:** Using custom delegates for Progress Bars and building a Gantt Chart view.
* **State Management:** Solving the "Offline-First" challenge and managing data consistency during synchronization.
* **Environment Management:** Setting up and managing a local development server using Docker.
