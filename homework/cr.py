from selenium import webdriver
from selenium.webdriver.chrome.options import Options

options = Options()

# Use a separate profile folder just for Selenium
options.add_argument("--user-data-dir=/Users/josephsaputra/selenium-chrome-profile")

# Optional: name a profile inside that folder
options.add_argument("--profile-directory=Profile 1")

# Keep browser open
options.add_experimental_option("detach", True)

driver = webdriver.Chrome(options=options)
driver.get("https://www.google.com")