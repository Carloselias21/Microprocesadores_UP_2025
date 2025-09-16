import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression

df = pd.read_csv("calibration.csv")
# Prepare X (ADC) and y (Moisture %)
X = df[["ADC Raw Value"]].values
y = df["%Moisture(Target)"].values
# Polynomial fit
poly = PolynomialFeatures(degree=2) # try degree=2,3
X_poly = poly.fit_transform(X)
model = LinearRegression()
model.fit(X_poly, y)
# Plot curve
plt.scatter(X, y, label="Measured")
X_fit = np.linspace(X.min(), X.max(), 200).reshape(-1, 1)
y_fit = model.predict(poly.transform(X_fit))
plt.plot(X_fit, y_fit, color="red", label="Fitted Curve")
plt.xlabel("ADC Raw Value")
plt.ylabel("Moisture %")
plt.legend()
plt.show()
# Export coefficients
print("Polynomial coefficients:", model.coef_)
print("Intercept:", model.intercept_)
