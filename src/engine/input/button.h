namespace engine {

	class Button {
	public:
		void tick(bool pressed);
		bool is_pressed() const;
		bool is_released() const;
		bool is_just_pressed() const;
		bool is_just_released() const;

	private:
		bool m_pressed;
		bool m_just_changed;
	};

} // namespace engine
