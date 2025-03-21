<p align="center">
  <img src="https://github.com/mld-khaki/edfbrowser_bids_mk/blob/main/edfbrowser_bids/images/splash.png" alt="EDFBrowser-BIDS"/>
</p>

# EDFBrowser-BIDS Pipeline Tool

This repository is a modified version of the open-source **EDFBrowser**. It integrates BIDS (Brain Imaging Data Structure) tools and command-line functionality to support automated pipelines.

## Key Features

- ✅ Command-line support for EDFBrowser actions (not supported in original EDFBrowser)
- ✅ EDF clipping with import and preservation of EDF header metadata
- ✅ Seamless integration with pipelines via command interface
- ✅ BIDS compatibility features for preprocessing workflows

## Usage

### Command-Line Interface

Example usage:
```
edfbrowser_pipeline --input yourfile.edf --clip 10:00-12:00 --output clipped_output.edf
```

### Parameters

| Flag           | Description                            |
|----------------|----------------------------------------|
| `--input`      | Path to the input EDF file             |
| `--clip`       | Time range to clip (e.g., 10:00-12:00) |
| `--output`     | Path to the output EDF file            |
| `--export-bids`| Export metadata in BIDS format         |

## Background

This project builds on **EDFBrowser** by adding essential automation and interoperability features required for modern neuroscience data pipelines.

## Acknowledgments

Original EDFBrowser: [https://www.teuniz.net/edfbrowser/](https://www.teuniz.net/edfbrowser/)

## License

Based on original EDFBrowser license. See `LICENSE.txt` for more info.
