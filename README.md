# Bad-Apple-in-CPP

Yo, what's up? Wanna watch the **Bad Apple** video... but in your terminal? Yeah, you heard that right. This bad boy converts the **Bad Apple** video (or any video, really) into **ASCII art** and lets you vibe to the audio in the background. It’s like a retro trip, but in your terminal. 😎

Get ready for some serious **meme magic**.

## Features 🎉

- **ASCII Art Video**: Watch the video in pure ASCII art. It’s pixelated, it’s retro, and it’s AWESOME.
- **Audio**: We didn’t forget the music. 🎶 The audio plays in the background while the video runs.
- **Terminal Resizing**: It auto-adjusts to your terminal size. Resize your window, and the video adapts. Flexin' in real time. ✨

## What You Need ⚙️

Before you get this meme show on the road, you’ll need:

- **FFmpeg**: We’re using it to extract video frames and audio. 🍿
- **libavformat, libavcodec, libavutil**: These are the FFmpeg libraries. Basically, they do all the heavy lifting. 💪
- **A Terminal**: Yeah, you need one of those. But you probably already knew that. 🖥️
- **C++11 or later**: Just make sure your compiler’s not stuck in 2002. 😅

### Dependencies Install 🍃

If you're on **Linux**:

```bash
sudo apt-get install libavformat-dev libavcodec-dev libavutil-dev

