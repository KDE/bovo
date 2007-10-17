add_subdirectory (scribble)
add_subdirectory (highcontrast)

# Spacy and Gomoku doesn't have white background color
# causing trouble on theme switching. Disable them for 4.0
# and solve POST-4.0
add_subdirectory (spacy)
add_subdirectory (gomoku)
