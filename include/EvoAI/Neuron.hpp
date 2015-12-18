#ifndef NEURON_HPP
#define NEURON_HPP

namespace EvoAI{
    /**
     * @brief EvoAI::Neuron class
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     */
    class Neuron final{
        public:
            /**
             * @brief Neuron::Type defines the type of a neuron.
             */
            enum Type{
                CONTEXT,
                HIDDEN,
                INPUT,
                OUTPUT
            };
        public:
            /**
             * @brief Default Constructor type defaults to Type::HIDDEN
             */
            Neuron();
            /**
             * @brief Constructor that takes a type parameter.
             * @param const Type& t
             */
            Neuron(const Type& t);
            /**
             * @brief type setter
             */
            Neuron& setType(const Type& t);
            /**
             * @brief type getter
             * @return Type&
             */
            inline const Type& getType() const{ return type; }
            /**
             * @brief get value
             * @return double&
             */
            inline const double& getValue() const{ return value; }
            /**
             * @brief Value setter sets oldvalue to value and value to new value.
             * @return Neuron&
             */
            Neuron& setValue(const double& val);
            /**
             * @brief oldValue getter
             * @return double&
             */
            inline const double& getOldValue() const{ return oldValue; }
            /**
             * @brief error getter
             * @return double&
             */
            inline const double& getError() const{ return error; }
            /**
             * @brief error setter
             * @param double& err
             * @return Neuron&
             */
            Neuron& setError(const double& err);
            /**
             * @brief resets the neuron if is not Type:CONTEXT
             * @return Neuron&
             */
            Neuron& reset();
            bool operator==(const Neuron& rhs) const;
            ~Neuron() = default;
        private:
            double value;
            double oldValue;
            double error;
            Type type;
    };
}

#endif // NEURON_HPP
