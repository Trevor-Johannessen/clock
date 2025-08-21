from fastapi import FastAPI, Depends, HTTPException, status
from fastapi.security import OAuth2PasswordBearer
import uvicorn

app = FastAPI()
VALID_TOKENS = {"mysecrettoken123"}
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

async def verify_token(token: str = Depends(oauth2_scheme)):
    if token not in VALID_TOKENS:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid or missing token",
            headers={"WWW-Authenticate": "Bearer"},
        )
    return token

@app.post("/token")
async def login():
    # Normally you'd validate a username/password and return a token.
    # For demo purposes, just return a hardcoded one.
    return {"access_token": "mysecrettoken123", "token_type": "bearer"}


@app.get("/protected")
async def protected_route(token: str = Depends(verify_token)):
    return {"message": f"Access granted with token: {token}"}

if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=8123)
