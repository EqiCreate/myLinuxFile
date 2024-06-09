import asyncio
import logging
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Configure logging
logging.basicConfig(filename='file_monitor.log', level=logging.INFO, format='%(asctime)s - %(message)s')

# Define the directory to monitor
directory_to_watch = '/usr/local/gitcode/myLinuxFile/mypython'

class FileChangeHandler(FileSystemEventHandler):
    def __init__(self):
        self.pending_changes = set()
        self.process_changes_task = None

    def on_modified(self, event):
        self.pending_changes.add(event.src_path)
        if self.process_changes_task is None or self.process_changes_task.done():
            self.process_changes_task = asyncio.create_task(self.process_changes())

    async def process_changes(self):
        await asyncio.sleep(5)  # Adjust the delay as needed
        for path in self.pending_changes:
            logging.info(f"File modified: {path}")
        self.pending_changes.clear()


async def monitor_directory():
    # Create an observer to monitor the directory
    observer = Observer()
    observer.schedule(FileChangeHandler(), directory_to_watch, recursive=True)
    observer.start()

    try:
        while True:
            await asyncio.sleep(1)  # Keep the event loop running
    except KeyboardInterrupt:
        observer.stop()

    observer.join()

async def main():
    await monitor_directory()

if __name__ == "__main__":
    # Ensure the event loop is running before executing asyncio tasks
    asyncio.run(main())
