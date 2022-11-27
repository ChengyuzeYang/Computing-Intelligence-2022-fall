import imageio
import os

def create_gif(image_list, gif_name, duration = 1.0):
    frames = []
    for image_name in image_list:
        frames.append(imageio.imread(image_name))
    
    imageio.mimsave(gif_name, frames, duration=duration)
    return

def main():
    image_list = []
    for file in os.listdir("."):
        s = file.split('.')
        if (s[1] == "py") | (s[1] == "gif"):
            continue
        image_list.append(file)
    gif_name = "iter.gif"
    duration = 0.01
    create_gif(image_list,gif_name,duration)

if __name__ == '__main__':
    main()