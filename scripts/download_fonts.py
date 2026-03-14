import requests
import sys
import os

def download_font(font_name, output_dir):
    # Strip .ttf if provided
    font_name = font_name.replace(".ttf", "")
    
    # Convert to Google Fonts format (e.g. RobotoMono -> Roboto Mono)
    import re
    spaced = re.sub(r'(?<=[a-z])(?=[A-Z])', ' ', font_name)
    
    # Try to get the font via Google Fonts CSS API
    url = f"https://fonts.googleapis.com/css2?family={spaced.replace(' ', '+')}"
    headers = {"User-Agent": "Mozilla/5.0"}  # Required or Google rejects the request
    response = requests.get(url, headers=headers)
    
    if response.status_code != 200:
        print(f"ERROR: Could not find font '{font_name}' on Google Fonts")
        sys.exit(1)

    # Extract the TTF url from the CSS response
    import re
    ttf_urls = re.findall(r'url\((https://[^)]+\.ttf)\)', response.text)
    if not ttf_urls:
        print(f"ERROR: No TTF URL found for '{font_name}'")
        sys.exit(1)

    ttf_url = ttf_urls[0]
    dest = os.path.join(output_dir, f"{font_name}.ttf")

    if not os.path.exists(dest):
        print(f"Downloading {font_name} from {ttf_url}")
        font_data = requests.get(ttf_url, headers=headers)
        with open(dest, "wb") as f:
            f.write(font_data.content)
    else:
        print(f"Font {font_name} already cached, skipping download")

    return dest

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("fonts", nargs="+")
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    os.makedirs(args.output, exist_ok=True)

    for font in args.fonts:
        path = download_font(font, args.output)
        print(path)