class enum ComponentState {
	Active;
	Asleep;
	Dead;
}

class Component {
public:
	virtual ~Component();	
	virtual void Update(float deltaTime) = 0;

private:
	ComponentState state_{Active};
}
