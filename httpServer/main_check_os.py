import os
import socket
import selectors  # Missing import for selectors

def check_system_features():
    features = {}

    # Check for os.fork
    features['os.fork'] = hasattr(os, 'fork')

    # Check for socket.AF_UNIX
    features['socket.AF_UNIX'] = hasattr(socket, 'AF_UNIX')


    # -----------------Selectors----------------------
    # Check for SelectSelector
    features['SelectSelector'] = hasattr(selectors, 'SelectSelector')

    # Check for PollSelector
    features['PollSelector'] = hasattr(selectors, 'PollSelector')

    # Check for EpollSelector (Linux only)
    features['EpollSelector'] = hasattr(selectors, 'EpollSelector')

    # Check for KqueueSelector (BSD/macOS only)
    features['KqueueSelector'] = hasattr(selectors, 'KqueueSelector')

    # Display the results
    print("System Features:")
    for feature, available in features.items():
        status = "Available" if available else "Not Available"
        print(f"- {feature}: {status}")

if __name__ == "__main__":
    print("Checking system features...")
    check_system_features()
